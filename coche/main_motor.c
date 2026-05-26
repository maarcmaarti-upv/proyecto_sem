/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    main_motor.c
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Nodo actuador y de captura del motor: procesa el setpoint por MQTT y monitoriza las RPM
*/

/* Includes ------------------------------------------------------------------*/
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

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define WIFI_SSID      "MOVISTAR_B780"
#define WIFI_PASSWORD  "rvp3M7w4tPyyvErv97p3"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "MQTT_UPV";

esp_mqtt_client_handle_t client = NULL;

static float motor_percent = 0.0f;
static int rpm_actual = 0;

/* Private function prototypes -----------------------------------------------*/
static void wifi_init(void);
static void mqtt_init(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void task_motor_tx_pwm(void *pv);
static void task_rpm_count(void *pv);
static void task_rpm_publish(void *pv);

/* Exported functions --------------------------------------------------------*/

/**
    @brief  Punto de entrada e inicialización de tareas asociadas a la gestión motora
*/
void app_main(void)
{
    ESP_LOGI("MAIN", "app_main arrancando");

    nvs_flash_init();
    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(15000));

    mqtt_init();
    motor_init();
    hall_rpm_init();

    /* Creación de hilos del planificador FreeRTOS */
    xTaskCreate(task_motor_tx_pwm, "task_motor_tx_pwm", 2048, NULL, 4, NULL);
    xTaskCreate(task_rpm_count,   "task_rpm_count",   2048, NULL, 4, NULL);
    xTaskCreate(task_rpm_publish, "task_rpm_publish", 2048, NULL, 3, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Private functions ---------------------------------------------------------*/

/**
    @brief  Enlaza el adaptador de red inalámbrica con el punto de acceso
*/
static void wifi_init(void)
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

/**
    @brief  Registra el callback de eventos e inicia el cliente de mensajería MQTT
*/
static void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.hivemq.com",
        .credentials.username = "giirob",
        .credentials.authentication.password = "UPV2024"
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/**
    @brief  Parsea las publicaciones del tópico del mando remoto para actualizar la velocidad del motor
*/
static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id) {

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT CONECTADO ");
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

/**
    @brief  Hilo de actualización síncrona del PWM del motor a una tasa de refresco fija de 20 Hz
*/
static void task_motor_tx_pwm(void *pv)
{
    while (1) {
        motor_set_power(motor_percent);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
    @brief  Hilo periódico encargado de interrogar al captador Hall cada 1000 ms
*/
static void task_rpm_count(void *pv)
{
    while (1) {
        rpm_actual = hall_rpm_get_rpm();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
    @brief  Hilo cíclico encargado de transmitir las RPM actuales del rotor al bróker MQTT
*/
static void task_rpm_publish(void *pv)
{
    char msg[16];

    while (1) {
        sprintf(msg, "%d", rpm_actual);
        esp_mqtt_client_publish(client, "proyecto/sem/rpm/valor", msg, 0, 1, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* End of file ****************************************************************/
