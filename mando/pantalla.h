#ifndef PANTALLA_LCD_H
#define PANTALLA_LCD_H

#include "driver/gpio.h"

// Definición de pines (ajusten según su cableado en el ESP32)
// Definición de pines para ESP32-S3
#define LCD_RS_GPIO    GPIO_NUM_1   // Pin RS
#define LCD_E_GPIO     GPIO_NUM_2   // Pin Enable
#define LCD_D4_GPIO    GPIO_NUM_4   // Pin D4
#define LCD_D5_GPIO    GPIO_NUM_5   // Pin D5
#define LCD_D6_GPIO    GPIO_NUM_6   // Pin D6
#define LCD_D7_GPIO    GPIO_NUM_7   // Pin D7

// Funciones principales
void lcd_init(void);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_send_string(char *str);
void lcd_put_cur(int row, int col);
void lcd_clear(void);
#endif