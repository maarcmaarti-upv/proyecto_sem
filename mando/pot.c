/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    pot.c
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Módulo ADC para la lectura del potenciómetro de entrada
*/

/* Includes ------------------------------------------------------------------*/
#include "pot.h"
#include "esp_adc/adc_oneshot.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define POT_ADC_UNIT   ADC_UNIT_1
#define POT_CHANNEL    ADC_CHANNEL_3   /* GPIO4 */
#define POT_ATTEN      ADC_ATTEN_DB_11

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Variable global estática privada de módulo con prefijo 's_' */
static adc_oneshot_unit_handle_t s_adc1;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void pot_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = POT_ADC_UNIT,
    };
    adc_oneshot_new_unit(&init_config, &s_adc1);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = POT_ATTEN,
    };

    adc_oneshot_config_channel(s_adc1, POT_CHANNEL, &config);
}

int32_t pot_read_raw(void)
{
    int val = 0;
    adc_oneshot_read(s_adc1, POT_CHANNEL, &val);
    
    /* El casteo explícito previene comportamientos indefinidos de anchos de tipo */
    return (int32_t)val;
}

/* Private functions ---------------------------------------------------------*/

/* End of file ****************************************************************/
