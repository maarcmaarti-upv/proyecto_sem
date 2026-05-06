/*** file encoder.c ****/

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "encoder.h"

#define ENCODER_GPIO GPIO_NUM_4
#define PULSOS_POR_REVOLUCION 20.0f 

static volatile int pulse_count = 0;


/* -------------------------------------------------- */
/* Inicialización del encoder como entrada digital */

void encoder_init(void)
{
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask = 1ULL << ENCODER_GPIO;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);
}

/* -------------------------------------------------- */
/* Contar pulsos del motoret */

static void encoder_task(void *pvParameters)
{
    int last_level = 0;
    int current_level;

    for (;;)
    {
        current_level = gpio_get_level(ENCODER_GPIO);

        /* Detecta flanco de subida */
        if (current_level == 1 && last_level == 0)
        {
            pulse_count++;
        }

        last_level = current_level;

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/* -------------------------------------------------- */
/* Devolver el número de pulsos y rpm */

int encoder_get_pulses(void)
{
    int pulses;

    taskENTER_CRITICAL(NULL);
    pulses = pulse_count;
    pulse_count = 0;
    taskEXIT_CRITICAL(NULL);

    return pulses;
}


float encoder_get_rpm(void)
{
    // Obtenemos los pulsos acumulados y reseteamos el contador
    int pulsos = encoder_get_pulses(); 
    
    float rpm = (pulsos / PULSOS_POR_REVOLUCION) * 120.0f;
    
    return rpm;
}

/* -------------------------------------------------- */
/* Crear tarea del encoder */

void encoder_start(void)
{
    xTaskCreate(
        encoder_task,
        "encoder_task",
        2048,
        NULL,
        2,
        NULL
    );
}

/*** End of file ****/
---------------------------------------------------------------------------
