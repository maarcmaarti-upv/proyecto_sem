/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    main_mando.c
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Módulo principal de control de telemetría por MQTT y FreeRTOS
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

#include "pot.h"
#include "lcd_i2c.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define WIFI_SSID      "MOVISTAR_B780"
#define WIFI_PASSWORD  "rvp3M7w4tPyyvErv97p3"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "MQTT_UPV";

esp_mqtt_client_handle_t client = NULL;

static int pot_value = 0;       /* Valor leído del ADC */
static int rpm_recibido = 0;    /* Valor recibido por MQTT */

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/* Private function prototypes -----------------------------------------------*/
static void wifi_init(void);
static void mqtt_init(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void task_pot_read(void *pv);
static void task_pot_publish(void *pv);
static void task_display_rpm(void *pv);

/* Exported functions --------------------------------------------------------*/

/**
    @brief  Punto de entrada de la aplicación firmware (FreeRTOS root task)
*/
void app_main(void)
{
    ESP_LOGI("MAIN", "app_main arrancando");

    nvs_flash_init();
    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(15000));   /* Sincronización de arranque */

    mqtt_init();
    pot_init();
    lcd_init();
    lcd_clear();

    xTaskCreate(task_pot_read,    "task_pot_read",    2048, NULL, 4, NULL);
    xTaskCreate(task_pot_publish, "task_pot_publish", 2048, NULL, 3, NULL);
    xTaskCreate(task_display_rpm, "task_display_rpm", 2048, NULL, 3, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Private functions ---------------------------------------------------------*/

/**
    @brief  Inicializa la pila de red y conecta la estación WiFi interna
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
    @brief  Configura las credenciales y arranca el demonio cliente de MQTT
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
    @brief  Manejador de eventos asíncronos para subscripciones e hilos MQTT
*/
static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id) {

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT CONECTADO");
        esp_mqtt_client_subscribe(event->client, "proyecto/sem/rpm/valor", 1);
        break;

    case MQTT_EVENT_DATA:
        if (strncmp(event->topic, "proyecto/sem/rpm/valor", event->topic_len) == 0) {

            char valor_str[16];
            size_t len = event->data_len;
            if (len > 15) len = 15;
            memcpy(valor_str, event->data, len);
            valor_str[len] = '\0';

            int rpm = atoi(valor_str);

            taskENTER_CRITICAL(&mux);
            rpm_recibido = rpm;
            taskEXIT_CRITICAL(&mux);

            ESP_LOGI(TAG, "RPM recibido = %d", rpm);
        }
        break;

    default:
        break;
    }
}

/**
    @brief  Tarea encargada de muestrear cíclicamente el potenciómetro hardware
*/
static void task_pot_read(void *pv)
{
    while (1) {
        int lectura = pot_read_raw();

        taskENTER_CRITICAL(&mux);
        pot_value = lectura;
        taskEXIT_CRITICAL(&mux);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/**
    @brief  Tarea encargada de empaquetar y publicar el valor analógico por MQTT
*/
static void task_pot_publish(void *pv)
{
    char msg[16];
    int valor;

    while (1) {
        taskENTER_CRITICAL(&mux);
        valor = pot_value;
        taskEXIT_CRITICAL(&mux);

        sprintf(msg, "%d", valor);

        esp_mqtt_client_publish(client, "proyecto/sem/pot/valor", msg, 0, 1, 0);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
    @brief  Tarea encargada de actualizar de forma síncrona el panel LCD 16x2
*/
static void task_display_rpm(void *pv)
{
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print(" RPM recibidos:");

    while (1) {
        int rpm;

        taskENTER_CRITICAL(&mux);
        rpm = rpm_recibido;
        taskEXIT_CRITICAL(&mux);

        lcd_set_cursor(0, 1);
        lcd_print("                ");

        lcd_set_cursor(0, 1);
        char buffer[16];
        sprintf(buffer, "%d", rpm);
        lcd_print(buffer);

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/* End of file ****************************************************************/
