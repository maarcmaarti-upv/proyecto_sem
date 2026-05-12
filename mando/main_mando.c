#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pot.h"
#include "mqtt_client.h"
#include "wifi_mqtt.h"

// Prioridades
#define POT_READ_TASK_PRIORITY      3
#define POT_PUBLISH_TASK_PRIORITY   3

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
