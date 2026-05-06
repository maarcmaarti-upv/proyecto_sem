#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define HALL_PIN GPIO_NUM_4

// PCNT
static volatile int pulse_count = 0;
static volatile int64_t last_time = 0;

// Buffer para media móvil
#define N 5
static int rpm_buffer[N] = {0};
static int idx = 0;
static int sum = 0;

// ISR con debounce temporal
static void IRAM_ATTR hall_isr_handler(void* arg)
{
    int64_t now = esp_timer_get_time(); // microsegundos

    // debounce de 5 ms
    if (now - last_time > 5000) {
        pulse_count++;
        last_time = now;
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << HALL_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(HALL_PIN, hall_isr_handler, NULL);

    printf("U18301 Hall RPM iniciado\n");

    while (1) {

        // Reiniciar contador
        pulse_count = 0;

        // Ventana de 1 segundo
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Copia segura del contador
        int pulses = pulse_count;

        // RPM
        int rpm = pulses * 60;

        // filtro
        sum -= rpm_buffer[idx];
        rpm_buffer[idx] = rpm;
        sum += rpm_buffer[idx];

        idx = (idx + 1) % N;

        int rpm_filtrado = sum / N;

        printf("Pulsos: %d | RPM: %d | RPM Filtrado: %d\n",
               pulses, rpm, rpm_filtrado);
    }
}
