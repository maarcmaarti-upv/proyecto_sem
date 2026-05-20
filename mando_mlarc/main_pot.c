#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "pot.h"
#include "wifi.h"
#include "mqtt_app.h"

void app_main(void)
{
    wifi_init();
    vTaskDelay(pdMS_TO_TICKS(5000));

    mqtt_app_start();
    vTaskDelay(pdMS_TO_TICKS(2000));

    pot_init();

    char msg[16];

    while (1) {
        int raw = pot_read_raw();

        snprintf(msg, sizeof(msg), "%d", raw);

        printf("Enviando: %s\n", msg);

        mqtt_publish("sem/pot", msg);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
