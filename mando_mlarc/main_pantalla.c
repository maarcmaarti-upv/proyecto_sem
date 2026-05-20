#include <stdio.h>
#include "lcd_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    lcd_init();
    lcd_clear();

    lcd_set_cursor(0, 0);
    lcd_print("Jaume parra");

    lcd_set_cursor(0, 1);
    lcd_print("menja penis");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
