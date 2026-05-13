#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "mqtt_client.h"
#include "wifi_mqtt.h"
#include "pantalla.h"

// Prioridades
#define IMPRIMIR_TASK_PRIORITY      3

// Periodos
#define IMPRIMIR_CYCLE_MS           500

// Buffer compartido
static char rpm_str[32] = "0000";

// Mutex para proteger rpm_str
static SemaphoreHandle_t mutex_rpm;

// Prototipos
static void imprimir_task(void *pvParameters);

// CALLBACK MQTT: se ejecuta cuando llega un mensaje
void mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {

        case MQTT_EVENT_DATA:
            if (strcmp(event->topic, "proyecto/rpm/valor") == 0) {

                xSemaphoreTake(mutex_rpm, portMAX_DELAY);

                snprintf(rpm_str, sizeof(rpm_str), "%.*s", event->data_len, event->data);

                xSemaphoreGive(mutex_rpm);
            }
            break;
    }
}

// TAREA 1: Imprime en la pantalla LCD
static void imprimir_task(void *pvParameters)
{
    char local_rpm[32];

    for (;;) {

        xSemaphoreTake(mutex_rpm, portMAX_DELAY);
        strcpy(local_rpm, rpm_str);
        xSemaphoreGive(mutex_rpm);

        lcd_put_cur(1, 0);
        lcd_send_string("RPM: ");
        lcd_send_string(local_rpm);

        vTaskDelay(pdMS_TO_TICKS(IMPRIMIR_CYCLE_MS));
    }
}

// MAIN
void app_main(void)
{
    mutex_rpm = xSemaphoreCreateMutex();

    wifi_init();
    lcd_init();

    // Mensaje inicial
    lcd_put_cur(0, 0);
    lcd_send_string("Coche Listo");

    xTaskCreate(imprimir_task,
                "imprimir_task",
                4096,
                NULL,
                IMPRIMIR_TASK_PRIORITY,
                NULL);
}
