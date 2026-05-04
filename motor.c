/*** file motor.c ****/

#include <stdio.h>
#include "driver/gpio.h"
#include "motor.h"

#define MOTOR_GPIO GPIO_NUM_10

void motor_init(void)
{
    gpio_config_t io_conf = {};
    
    io_conf.pin_bit_mask = 1ULL << MOTOR_GPIO;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&io_conf);

    // Motor inicialmente apagado
    gpio_set_level(MOTOR_GPIO, 0);
}

void motor_set_power(float percent)
{
    if (percent > 0.0f)
    {
        gpio_set_level(MOTOR_GPIO, 1);
    }
    else
    {
        gpio_set_level(MOTOR_GPIO, 0);
    }
}

/*** End of file ****/
