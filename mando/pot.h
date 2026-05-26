/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    pot.h
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Template de cabecera para el módulo del potenciómetro
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef POT_H
#define POT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
int pot_read_raw(void);
void pot_init(void);

#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/
