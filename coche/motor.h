/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    motor.h
    @author  Grupo PCNT
    @version V0.4
    @date    2026-05-26
    @brief   Template de cabecera para la etapa de potencia del motor DC mediante PWM
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
void motor_init(void);
void motor_set_power(float percent);

#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/
