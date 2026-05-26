/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    hall_rpm.c
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Cálculo de velocidad angular y filtro de media móvil para el captador Hall
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "hall_rpm.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define HALL_GPIO GPIO_NUM_4
#define PCNT_UNIT PCNT_UNIT_0
#define PCNT_CHANNEL PCNT_CHANNEL_0

/* Media móvil */
#define N 5

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static int rpm_buffer[N] = {0};
static int idx = 0;
static int sum = 0;

static int last_rpm = 0;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
    @brief  Configura la unidad del contador de pulsos (PCNT) por hardware y su filtro antirrebotes
*/
void hall_rpm_init(void)
{
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = HALL_GPIO,
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,
        .channel = PCNT_CHANNEL,
        .unit = PCNT_UNIT,
        .pos_mode = PCNT_COUNT_INC,
        .neg_mode = PCNT_COUNT_DIS,
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = 10000,
        .counter_l_lim = 0
    };

    pcnt_unit_config(&pcnt_config);

    /* Filtro hardware (5 ms) */
    pcnt_set_filter_value(PCNT_UNIT, 1023);
    pcnt_filter_enable(PCNT_UNIT);

    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);
    pcnt_counter_resume(PCNT_UNIT);
}

/**
    @brief  Obtiene los pulsos acumulados, calcula la velocidad instantánea y aplica media móvil
    @retval Último valor de RPM filtrado
*/
int hall_rpm_get_rpm(void)
{
    int16_t pulses = 0;

    /* Leer pulsos acumulados */
    pcnt_get_counter_value(PCNT_UNIT, &pulses);

    /* Limpiar para siguiente ventana temporal */
    pcnt_counter_clear(PCNT_UNIT);

    /* Calcular RPM brutas */
    int rpm = pulses * 60;

    /* Algoritmo iterativo de media móvil */
    sum -= rpm_buffer[idx];
    rpm_buffer[idx] = rpm;
    sum += rpm_buffer[idx];
    idx = (idx + 1) % N;

    last_rpm = sum / N;

    return last_rpm;
}

/* Private functions ---------------------------------------------------------*/

/* End of file ****************************************************************/
