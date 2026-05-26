/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    pot.h
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Librería para la lectura limpia del potenciómetro usando tipos fijos
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef POT_H
#define POT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
void pot_init(void);
int32_t pot_read_raw(void);

#ifdef __cplusplus
}
#endif

#endif /* POT_H */
/*** End of file ***/
