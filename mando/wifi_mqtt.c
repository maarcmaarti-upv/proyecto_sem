#include "wifi_mqtt.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"

#define WIFI_SSID "iPhone" //tu wifi
#define WIFI_PASS "trufanala2" //tu contraseña

// configuración del mqtt explorer
#define MQTT_URI  "mqtt://broker.hivemq.com"
#define MQTT_USER "giirob"
#define MQTT_PASS "UPV2024"

static const char *TAG = "WIFI_MQTT";

// ---------------- WIFI ----------------
void wifi_init(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI(TAG, "Conectando a WiFi...");
}

// ---------------- MQTT ----------------
esp_mqtt_client_handle_t mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_URI,
        .credentials.username = MQTT_USER,
        .credentials.authentication.password = MQTT_PASS,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "MQTT iniciado");

    return client;
}
