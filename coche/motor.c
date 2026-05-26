/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    motor.c
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Configuración del periférico LEDC (PWM) para modular la potencia del motor
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "motor.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define MOTOR_GPIO         GPIO_NUM_10
#define MOTOR_PWM_TIMER    LEDC_TIMER_0
#define MOTOR_PWM_MODE     LEDC_LOW_SPEED_MODE
#define MOTOR_PWM_CHANNEL  LEDC_CHANNEL_0
#define MOTOR_PWM_FREQ     5000              /* Frecuencia PWM establecida a 5 kHz */
#define MOTOR_PWM_RES      LEDC_TIMER_10_BIT /* Resolución de 10 bits (rango dinámico 0-1023) */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
    @brief  Inicializa y enlaza el temporizador del LEDC con el canal del puente H asignado al motor
*/
void motor_init(void)
{
    /* Configuración del timer PWM */
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = MOTOR_PWM_MODE,
        .timer_num        = MOTOR_PWM_TIMER,
        .duty_resolution  = MOTOR_PWM_RES,
        .freq_hz          = MOTOR_PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    /* Configuración del canal PWM */
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

/**
    @brief  Satura el porcentaje de control y actualiza el ciclo de trabajo del PWM del hardware
    @param  percent Factor de marcha flotante comprendido entre 0.0f y 1.0f
*/
void motor_set_power(float percent)
{
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    /* Calcula el duty cycle entero según la resolución de 10 bits */
    uint32_t duty = (uint32_t)(percent * 1023); 
    ledc_set_duty(MOTOR_PWM_MODE, MOTOR_PWM_CHANNEL, duty);
    ledc_update_duty(MOTOR_PWM_MODE, MOTOR_PWM_CHANNEL);
}

/* Private functions ---------------------------------------------------------*/

/* End of file ****************************************************************/
