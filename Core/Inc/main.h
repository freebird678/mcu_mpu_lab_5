/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
  /* USER CODE END Includes */

  /* Exported types ------------------------------------------------------------*/
  /* USER CODE BEGIN ET */
  typedef struct
  {
    volatile bool flag;
    volatile uint16_t counter;
    volatile uint16_t duration;
  } swTim_t;

  typedef enum
  {
    EVT_NONE = 0,
    EVT_STATE_ENTRY,
    EVT_STATE_EXIT,
    EVT_CMD_RST,
    EVT_CMD_OK,
    EVT_TIMEOUT,
    EVT_MSG_RCVD,
  } sys_event;

  extern swTim_t led_tim;
  extern swTim_t led_pwm_tim;
  extern swTim_t arq_tim;
  extern sys_event app_evt;
  /* USER CODE END ET */

  /* Exported constants --------------------------------------------------------*/
  /* USER CODE BEGIN EC */
#define TIM2_TICK 1U          // milliseconds
#define LED_PERIOD 500U       // milliseconds
#define LED_PWM_DURATION 250U // 50% duty cycle, milliseconds
#define ARQ_TIMEOUT 3000U     // 3 seconds timeout
#define TX_BUF_SIZE 64
#define RX_BUF_SIZE 64
  /* USER CODE END EC */

  /* Exported macro ------------------------------------------------------------*/
  /* USER CODE BEGIN EM */

  /* USER CODE END EM */

  /* Exported functions prototypes ---------------------------------------------*/
  void Error_Handler(void);

  /* USER CODE BEGIN EFP */
  void swTim_init(swTim_t *timer, uint16_t duration);
  void swTim_clear(swTim_t *timer);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOA

  /* USER CODE BEGIN Private defines */

  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
