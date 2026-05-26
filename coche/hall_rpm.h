/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    hall_rpm.h
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Template de cabecera para la medición de RPM mediante sensor Hall
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HALL_RPM_H
#define HALL_RPM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "driver/pcnt.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
void hall_rpm_init(void);
int hall_rpm_get_rpm(void);

#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/
