// MICRO-PROJECTE DE SEM -> MOTORET


/*** file main.c ****/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor.h"
#include "encoder.h"
#include "mqtt_client.h"

esp_mqtt_client_handle_t client;



#define MOTOR_CONTROL_TASK_PRIORITY  3
#define MOTOR_CONTROL_CYCLE_TIME_MS  500

volatile float motor_power_percent = 100.0f;
static portMUX_TYPE motor_access = portMUX_INITIALIZER_UNLOCKED;

static TaskHandle_t motor_control_task_handle = NULL;

static void motor_control_task(void *pvParameters);

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    
}

void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://tu_servidor_mqtt.com", // O la IP de tu PC
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    mqtt_app_start();
    motor_init();
    encoder_start();

    xTaskCreate(
        motor_control_task,
        "motor_control_task",
        2048,
        NULL,
        MOTOR_CONTROL_TASK_PRIORITY,
        &motor_control_task_handle
    );

    for (;;)
    {
        vTaskDelay(300 / portTICK_PERIOD_MS);

        taskENTER_CRITICAL(&motor_access);
        motor_power_percent -= 0.3f;
        if (motor_power_percent < 0.0f)
        {
            motor_power_percent = 100.0f;
        }
        taskEXIT_CRITICAL(&motor_access);
    }
}

void motor_control_task(void *pvParameters)
{
    float power;
    float rpm;
    char rpm_str[16];
    
    int pulses = encoder_get_pulses();
    printf("Pulsos en el periodo: %d\n", pulses);


    for (;;)
    {
        taskENTER_CRITICAL(&motor_access);
        power = motor_power_percent;
        taskEXIT_CRITICAL(&motor_access);

        motor_set_power(power);
        rpm = encoder_get_rpm(); // [cite: 9]

        // 1. Convertir float a string
        sprintf(rpm_str, "%.2f", rpm);

        // 2. Publicar por MQTT
        // Argumentos: (cliente, topic, data, tamaño, qos, retain)
        esp_mqtt_client_publish(client, "/proyecto/motor/rpm", rpm_str, 0, 1, 0);

        printf("Potencia: %.1f %% | Velocidad: %.2f RPM (Enviado a MQTT)\n", power, rpm);

        vTaskDelay(MOTOR_CONTROL_CYCLE_TIME_MS / portTICK_PERIOD_MS); // 
    }
}

/*** End of file ****/


