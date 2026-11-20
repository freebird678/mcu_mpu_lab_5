/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
typedef struct
{
  uint8_t rx_byte;
  char tx_buf[TX_BUF_SIZE];
  char rx_buf[RX_BUF_SIZE];
  char out_buf[RX_BUF_SIZE];
  uint16_t rx_index;
  volatile uint8_t tx_busy;
  volatile uint8_t rx_ready;
} uart2_ctx_t;

static uart2_ctx_t uart2_ctx = {0};
/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspInit 1 */

    /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

  if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspDeInit 0 */

    /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspDeInit 1 */

    /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void uart2_rx_init(void)
{
  HAL_UART_Receive_IT(&huart2, &uart2_ctx.rx_byte, 1);
}

const char *uart2_receive(void)
{
  if (!uart2_ctx.rx_ready)
  {
    return NULL;
  }

  memcpy(uart2_ctx.out_buf, uart2_ctx.rx_buf, RX_BUF_SIZE);
  uart2_ctx.rx_ready = 0;

  return uart2_ctx.out_buf;
}

uint8_t uart2_send(const char *msg)
{
  if (msg == NULL || uart2_ctx.tx_busy)
  {
    return 0;
  }

  size_t len = strlen(msg);
  if (len == 0)
  {
    return 0;
  }
  if (len >= TX_BUF_SIZE)
  {
    len = TX_BUF_SIZE - 1;
  }

  memcpy(uart2_ctx.tx_buf, msg, len);
  uart2_ctx.tx_busy = 1;

  if (HAL_UART_Transmit_IT(&huart2, (uint8_t *)uart2_ctx.tx_buf, (uint16_t)len) != HAL_OK)
  {
    uart2_ctx.tx_busy = 0;
    return 0;
  }

  return 1;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    uart2_ctx.tx_busy = 0;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    if (!uart2_ctx.rx_ready)
    {
      if (uart2_ctx.rx_byte == '\r' || uart2_ctx.rx_byte == '\n')
      {
        /* Ignore line endings; commands are framed by '!' and '#' */
      }
      else if (uart2_ctx.rx_byte == '!')
      {
        uart2_ctx.rx_index = 0;
        uart2_ctx.rx_buf[uart2_ctx.rx_index++] = '!';
      }
      else if (uart2_ctx.rx_byte == '#')
      {
        if (uart2_ctx.rx_index < (RX_BUF_SIZE - 1))
        {
          uart2_ctx.rx_buf[uart2_ctx.rx_index++] = '#';
        }
        uart2_ctx.rx_buf[uart2_ctx.rx_index] = '\0';
        uart2_ctx.rx_ready = 1;
        uart2_ctx.rx_index = 0;
      }
      else if (uart2_ctx.rx_index < (RX_BUF_SIZE - 1))
      {
        uart2_ctx.rx_buf[uart2_ctx.rx_index++] = (char)uart2_ctx.rx_byte;
      }
    }
    HAL_UART_Receive_IT(&huart2, &uart2_ctx.rx_byte, 1);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
    __HAL_UART_CLEAR_OREFLAG(&huart2);
    HAL_UART_Receive_IT(&huart2, &uart2_ctx.rx_byte, 1);
  }
}
/* USER CODE END 1 */
