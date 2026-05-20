/*** file main.c ***/

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

#include "motor.h"
#include "hall_rpm.h"

static const char *TAG = "MQTT_UPV";

// WiFi

#define WIFI_SSID      "MOVISTAR_B780"
#define WIFI_PASSWORD  "rvp3M7w4tPyyvErv97p3"

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

// MQTT

esp_mqtt_client_handle_t client = NULL;

// Variable global del motor
static float motor_percent = 0.0f;

// Variable global del RPM
static int rpm_actual = 0;

static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT CONECTADO ");

            // Suscripción al potenciómetro del mando
            esp_mqtt_client_subscribe(event->client, "proyecto/sem/pot/valor", 1);
            break;

        case MQTT_EVENT_DATA:
            if (strncmp(event->topic, "proyecto/sem/pot/valor", event->topic_len) == 0) {

                char valor_str[16];
                size_t len = event->data_len;
                if (len > 15) len = 15;
                memcpy(valor_str, event->data, len);
                valor_str[len] = '\0';

                int valor_adc = atoi(valor_str);
                motor_percent = ((float)valor_adc) / 4095.0f;

                if (motor_percent < 0.0f) motor_percent = 0.0f;
                if (motor_percent > 1.0f) motor_percent = 1.0f;

                ESP_LOGI(TAG, "Potenciómetro recibido ADC=%d, porcentaje=%.2f",
                         valor_adc, motor_percent);
            }
            break;

        default:
            break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.hivemq.com",
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

// TAREAS DEL MOTOR

// Tarea que recibe el potenciómetro
// Esta tarea aplica el valor al motor
void task_motor_tx_pwm(void *pv)
{
    while (1) {
        motor_set_power(motor_percent);
        vTaskDelay(pdMS_TO_TICKS(50)); // 20 Hz
    }
}

/* -------------------------------------------------- */
//TAREAS DEL RPM

// Tarea que calcula RPM cada 1 segundo
void task_rpm_count(void *pv)
{
    while (1) {
        rpm_actual = hall_rpm_get_rpm();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Tarea que publica RPM por MQTT
void task_rpm_publish(void *pv)
{
    char msg[16];

    while (1) {
        sprintf(msg, "%d", rpm_actual);
        esp_mqtt_client_publish(client, "proyecto/sem/rpm/valor", msg, 0, 1, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* -------------------------------------------------- */

void app_main(void)
{
    ESP_LOGI("MAIN", "app_main arrancando");

    nvs_flash_init();
    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(15000));

    mqtt_init();
    motor_init();
    hall_rpm_init();

    // TAREAS DEL MOTOR

    xTaskCreate(task_motor_tx_pwm, "task_motor_tx_pwm", 2048, NULL, 4, NULL);

    // TAREAS DEL RPM

    xTaskCreate(task_rpm_count,   "task_rpm_count",   2048, NULL, 4, NULL);
    xTaskCreate(task_rpm_publish, "task_rpm_publish", 2048, NULL, 3, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*** End of file ***/
