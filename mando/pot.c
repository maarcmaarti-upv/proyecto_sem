#include "pot.h"
#include "esp_adc/adc_oneshot.h"

#define POT_ADC_UNIT    ADC_UNIT_1
#define POT_CHANNEL     ADC_CHANNEL_3   // GPIO4
#define POT_ATTEN       ADC_ATTEN_DB_11

static adc_oneshot_unit_handle_t adc1;

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

int pot_read_raw(void)
{
    int val = 0;
    adc_oneshot_read(adc1, POT_CHANNEL, &val);
    return val;
}
