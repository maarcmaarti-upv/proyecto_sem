#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "mqtt_client.h"

#include "motor.h

static const char *TAG = "MQTT_UPV";

/* -------------------------------------------------- */
/* WiFi */

#define WIFI_SSID      "UPVNET"
#define WIFI_PASSWORD  "marcmarti"

void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        }
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI("WIFI", "Intentando conectar a la red WIFI...");
}

/* -------------------------------------------------- */
/* MQTT */

static esp_mqtt_client_handle_t client;

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT CONECTADO ");
        // Nos suscribimos al topic del potenciómetro del mando.
        esp_mqtt_client_subscribe(event->client, "proyecto/pot/valor", 1);
        break;

    case MQTT_EVENT_DATA: {
        // Solo actuamos si el topic es el esperado
        if (strncmp(event->topic, "proyecto/pot/valor", event->topic_len) == 0) {
            char valor_str[16];
            size_t len = event->data_len;
            if (len > 15) len = 15;
            memcpy(valor_str, event->data, len);
            valor_str[len] = '\0';

            float valor = atof(valor_str); // Convierte el string a float
            ESP_LOGI(TAG, "Potencia recibida: %.2f", valor);

            // Aplicamos ese valor al motor
            motor_set_power(valor);
        }
        break;
      }

    default:
        break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://mqtt.dsic.upv.es",
        .credentials.username = "giirob",
        .credentials.authentication.password = "UPV2024"
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client,
                                   ESP_EVENT_ANY_ID,
                                   mqtt_event_handler,
                                   NULL);
    esp_mqtt_client_start(client);
}


/* -------------------------------------------------- */

void app_main(void)
{
    ESP_LOGI("MAIN", "app_main arrancando");

    nvs_flash_init();
    wifi_init();

    
    vTaskDelay(pdMS_TO_TICKS(15000));

    mqtt_init();

    // Inicializamos el motor
    motor_init();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
}
