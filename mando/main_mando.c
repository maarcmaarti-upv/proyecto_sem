#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pot.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "wifi_mqtt.h"
#include <string.h>


// Prioridades
#define POT_READ_TASK_PRIORITY      3
#define POT_PUBLISH_TASK_PRIORITY   3
#define LDC_TASK_PRIORITY 3

// Periodos
#define POT_READ_CYCLE_MS           200
#define POT_PUBLISH_CYCLE_MS        500

// Variable compartida
volatile float pot_value = 0.0f;

// Mutex
static portMUX_TYPE pot_access = portMUX_INITIALIZER_UNLOCKED;

// Prototipos
static void pot_read_task(void *pvParameters);
static void pot_publish_task(void *pvParameters);
static void ldc_task(void * pvParameters);

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
            esp_mqtt_client_subscribe(event->client, "proyecto/sem/rpm/valor", 1);
            break;

        case MQTT_EVENT_DATA:
            if (strncmp(event->topic, "proyecto/sem/rpm/valor", event->topic_len) == 0) {

                char valor_str[16];
                size_t len = event->data_len;
                if (len > 15) len = 15;
                memcpy(valor_str, event->data, len);
                valor_str[len] = '\0';

                int valor_rpm = atoi(valor_str);
                ESP_LOGI(TAG, "Potenciómetro recibido RPM=%d.", valor_rpm);
                
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

// TAREA DE LECTURA DEL POTENCIÓMETRO
static void pot_read_task(void *pvParameters)
{
    for (;;) {
        float lectura = pot_read_raw();

        taskENTER_CRITICAL(&pot_access);
        pot_value = lectura;
        taskEXIT_CRITICAL(&pot_access);

        vTaskDelay(pdMS_TO_TICKS(POT_READ_CYCLE_MS));
    }
}

// TAREA DE PUBLICACIÓN MQTT
static void pot_publish_task(void *pvParameters)
{
    char pot_str[16];

    for (;;) {

        // Si MQTT aún no está inicializado -> esperar
        if (client == NULL) {
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        float valor;

        taskENTER_CRITICAL(&pot_access);
        valor = pot_value;
        taskEXIT_CRITICAL(&pot_access);

        sprintf(pot_str, "%.2f", valor);
        // función de publicar
        esp_mqtt_client_publish(client,
                                "proyecto/pot/valor",
                                pot_str,
                                0,
                                1,
                                0);

        printf("Potenciómetro: %.2f (MQTT)\n", valor);

        vTaskDelay(pdMS_TO_TICKS(POT_PUBLISH_CYCLE_MS));
    }
}

static void ldc_task(void *pvParameters) {
    // Escribir en la primera fila
    lcd_put_cur(0, 0);
    lcd_send_string("RPM: ");

    // Escribir en la segunda fila
    lcd_put_cur(1, 0);
    lcd_send_string("%d", valor_rpm);
}


// MAIN
void app_main(void)
{
    // Iniciar WiFi (MQTT se iniciará automáticamente al obtener IP)
    wifi_init();

    // Inicializar potenciómetro
    pot_init();

    // Crear tareas
    xTaskCreate(pot_read_task,
                "pot_read_task",
                2048,
                NULL,
                POT_READ_TASK_PRIORITY,
                NULL);

    xTaskCreate(pot_publish_task,
                "pot_publish_task",
                4096,   // más stack para evitar overflow
                NULL,
                POT_PUBLISH_TASK_PRIORITY,
                NULL);
    
    xtaskCreate(ldc_task, "ldc_task", 2048, NULL, LDC_TASK_PRIORITY, NULL);
}



