/*** file motor.c ****/

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "motor.h"

// Ajusta el pin según tu cableado real
#define MOTOR_GPIO GPIO_NUM_10

// Parámetros del PWM
#define MOTOR_PWM_TIMER      LEDC_TIMER_0
#define MOTOR_PWM_MODE       LEDC_HIGH_SPEED_MODE
#define MOTOR_PWM_CHANNEL    LEDC_CHANNEL_0
#define MOTOR_PWM_FREQ       5000             // Frecuencia PWM, por ejemplo 5 kHz
#define MOTOR_PWM_RES        LEDC_TIMER_10_BIT // Resolución de 10 bits (0-1023)

void motor_init(void)
{
    // Configuración del timer PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = MOTOR_PWM_MODE,
        .timer_num        = MOTOR_PWM_TIMER,
        .duty_resolution  = MOTOR_PWM_RES,
        .freq_hz          = MOTOR_PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configuración del canal PWM
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = MOTOR_PWM_MODE,
        .channel        = MOTOR_PWM_CHANNEL,
        .timer_sel      = MOTOR_PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_GPIO,
        .duty           = 0,
        .hpoint         = 0,
    };
    ledc_channel_config(&ledc_channel);
}

void motor_set_power(float percent)
{
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    // Calcula el duty cycle según el porcentaje
    uint32_t duty = (uint32_t)(percent * 1023); // 1023 para 10 bits
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_PWM_CHANNEL, duty);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_PWM_CHANNEL);
}

/*** End of file ****/
