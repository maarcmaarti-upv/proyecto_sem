/*** file hall_rpm.c ***/

#include <stdio.h>
#include "hall_rpm.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define HALL_GPIO GPIO_NUM_4
#define PCNT_UNIT PCNT_UNIT_0
#define PCNT_CHANNEL PCNT_CHANNEL_0

// Media móvil
#define N 5
static int rpm_buffer[N] = {0};
static int idx = 0;
static int sum = 0;

static int last_rpm = 0;

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

    // Filtro hardware (5 ms)
    pcnt_set_filter_value(PCNT_UNIT, 5000);
    pcnt_filter_enable(PCNT_UNIT);

    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);
    pcnt_counter_resume(PCNT_UNIT);
}

int hall_rpm_get_rpm(void)
{
    int16_t pulses = 0;

    // Leer pulsos
    pcnt_get_counter_value(PCNT_UNIT, &pulses);

    // Limpiar para siguiente ventana
    pcnt_counter_clear(PCNT_UNIT);

    // Calcular RPM
    int rpm = pulses * 60;

    // Media móvil
    sum -= rpm_buffer[idx];
    rpm_buffer[idx] = rpm;
    sum += rpm_buffer[idx];
    idx = (idx + 1) % N;

    last_rpm = sum / N;

    return last_rpm;
}
