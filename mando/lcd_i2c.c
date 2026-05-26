/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    lcd_i2c.c
    @author  Angel Perles / Modificado por Estudiante
    @version V0.4
    @date    2026-05-26
    @brief   Implementación del controlador LCD I2C respetando BARR-C:2018
*/

/* Includes ------------------------------------------------------------------*/
#include "lcd_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define I2C_PORT I2C_NUM_0
#define SDA_PIN  8
#define SCL_PIN  9

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void lcd_send_cmd(uint8_t cmd);
static void lcd_send_data(uint8_t data);
static void lcd_send(uint8_t value, uint8_t mode);
static void i2c_master_init(void);

/* Exported functions --------------------------------------------------------*/

void lcd_init(void)
{
    i2c_master_init();
    vTaskDelay(pdMS_TO_TICKS(50));

    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x01);

    vTaskDelay(pdMS_TO_TICKS(5));
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void lcd_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t offsets[] = {0x00, 0x40};
    
    /* Espacios reglamentarios en operaciones binarias y sin espacios en corchetes */
    lcd_send_cmd(0x80 | (col + offsets[row]));
}

void lcd_print(char const * p_str)
{
    /* Puntero con prefijo 'p_' según Barr 7.1.k */
    while (*p_str)
    {
        lcd_send_data(*p_str);
        p_str++;
    }
}

/* Private functions ---------------------------------------------------------*/

static void i2c_master_init(void)
{
    /* Nombres alineados visualmente en la declaración de estructura */
    i2c_config_t conf = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = SDA_PIN,
        .scl_io_num       = SCL_PIN,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };

    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

static void lcd_send_cmd(uint8_t cmd)
{
    lcd_send(cmd, 0);
}

static void lcd_send_data(uint8_t data)
{
    lcd_send(data, 1);
}

static void lcd_send(uint8_t value, uint8_t mode)
{
    uint8_t high = value & 0xF0;
    uint8_t low  = (value << 4) & 0xF0;

    /* Operador ternario formateado con espacios estrictos */
    uint8_t data_h = high | (mode ? 0x01 : 0x00) | 0x08;
    uint8_t data_l = low  | (mode ? 0x01 : 0x00) | 0x08;

    uint8_t seq[4] = {
        data_h | 0x04,
        data_h & ~0x04,
        data_l | 0x04,
        data_l & ~0x04
    };

    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (LCD_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd_handle, seq, 4, true);
    i2c_master_stop(cmd_handle);
    i2c_master_cmd_begin(I2C_PORT, cmd_handle, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd_handle);
}

/* End of file ****************************************************************/
