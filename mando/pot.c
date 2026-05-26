/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    pot.c
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Implementación de lecturas analógicas del potenciómetro
*/

/* Includes ------------------------------------------------------------------*/
#include "pot.h"
#include "esp_adc/adc_oneshot.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define POT_ADC_UNIT    ADC_UNIT_1
#define POT_CHANNEL     ADC_CHANNEL_3   /* GPIO4 */
#define POT_ATTEN       ADC_ATTEN_DB_11

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static adc_oneshot_unit_handle_t adc1;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
    @brief  Inicializa la unidad ADC1 y configura el canal del potenciómetro
*/
void pot_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = POT_ADC_UNIT,
    };
    adc_oneshot_new_unit(&init_config, &adc1);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = POT_ATTEN,
    };

    adc_oneshot_config_channel(adc1, POT_CHANNEL, &config);
}

/**
    @brief  Realiza una lectura analógica instantánea de un único disparo
    @retval Valor bruto (raw) leído del conversor ADC
*/
int pot_read_raw(void)
{
    int val = 0;
    adc_oneshot_read(adc1, POT_CHANNEL, &val);
    return val;
}

/* Private functions ---------------------------------------------------------*/

/* End of file ****************************************************************/
