#include "pantalla.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

static void lcd_toggle_enable(void) {
    gpio_set_level(LCD_E_GPIO, 1);
    ets_delay_us(1); 
    gpio_set_level(LCD_E_GPIO, 0);
    ets_delay_us(100);
}

static void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    gpio_set_level(LCD_RS_GPIO, rs);
    gpio_set_level(LCD_D4_GPIO, (nibble >> 0) & 0x01);
    gpio_set_level(LCD_D5_GPIO, (nibble >> 1) & 0x01);
    gpio_set_level(LCD_D6_GPIO, (nibble >> 2) & 0x01);
    gpio_set_level(LCD_D7_GPIO, (nibble >> 3) & 0x01);
    lcd_toggle_enable();
}

void lcd_send_cmd(uint8_t cmd) {
    lcd_send_nibble(cmd >> 4, 0);
    lcd_send_nibble(cmd & 0x0F, 0);
    vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_send_data(uint8_t data) {
    lcd_send_nibble(data >> 4, 1);
    lcd_send_nibble(data & 0x0F, 1);
    ets_delay_us(50);
}

void lcd_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LCD_RS_GPIO) | (1ULL << LCD_E_GPIO) |
                        (1ULL << LCD_D4_GPIO) | (1ULL << LCD_D5_GPIO) |
                        (1ULL << LCD_D6_GPIO) | (1ULL << LCD_D7_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0, .pull_down_en = 0, .intr_type = 0
    };
    gpio_config(&io_conf);

    vTaskDelay(50 / portTICK_PERIOD_MS);
    lcd_send_nibble(0x03, 0);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    lcd_send_nibble(0x03, 0);
    ets_delay_us(150);
    lcd_send_nibble(0x03, 0);
    lcd_send_nibble(0x02, 0);

    lcd_send_cmd(0x28); // 4-bit, 2 líneas, 5x8
    lcd_send_cmd(0x0C); // Display ON
    lcd_send_cmd(0x06); // Incremento cursor
    lcd_send_cmd(0x01); // Clear
    vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_put_cur(int row, int col) {
    uint8_t pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_send_cmd(pos);
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data((uint8_t)*str++);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
}



/*
main de prueba

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pantalla.h"

void app_main(void) {
    // Inicialización obligatoria
    lcd_init();

    // Escribir en la primera fila
    lcd_put_cur(0, 0);
    lcd_send_string("Coche S3 Listo");

    // Escribir en la segunda fila
    lcd_put_cur(1, 0);
    lcd_send_string("RPM: 0000");

    while (1) {
        // Aquí podrías actualizar las RPM más adelante
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}*/