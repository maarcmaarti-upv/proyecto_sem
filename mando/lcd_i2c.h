/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    lcd_i2c.h
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Template de cabecera aplicado al controlador LCD I2C
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LCD_I2C_H
#define LCD_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "driver/i2c.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define LCD_I2C_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);

#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/
