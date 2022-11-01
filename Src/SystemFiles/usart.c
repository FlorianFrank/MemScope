/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "SystemFiles/usart.h"
#include "error_handler.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_usart1_rx;
/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 2000000;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}
// my comment

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }else if(uartHandle->Instance==USART1)
  {
      /* USER CODE BEGIN USART1_MspInit 0 */

      /* USER CODE END USART1_MspInit 0 */
      /* Peripheral clock enable */
      __HAL_RCC_USART1_CLK_ENABLE();

      __HAL_RCC_GPIOA_CLK_ENABLE();

      /**USART1 GPIO Configuration
      PA9     ------> USART1_TX
      PA10     ------> USART1_RX
      */
      GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      hdma_usart1_rx.Instance = DMA2_Stream2;
      hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
      hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
      hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      hdma_usart1_rx.Init.Mode = DMA_NORMAL;
      hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
      hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
      {
          //printf("Error stm32fxx_hal_msp.c");
          Error_Handler();
      }

      __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

      /* USART1 interrupt Init */
      HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(USART1_IRQn);
      /* USER CODE BEGIN USART1_MspInit 1 */

      /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
 }else if(uartHandle->Instance==USART1)
  {
      /* USER CODE BEGIN USART1_MspDeInit 0 */

      /* USER CODE END USART1_MspDeInit 0 */
      /* Peripheral clock disable */
      __HAL_RCC_USART1_CLK_DISABLE();

      /**USART1 GPIO Configuration
      PA9     ------> USART1_TX
      PA10     ------> USART1_RX
      */
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

      /* USART1 DMA DeInit */
      HAL_DMA_DeInit(uartHandle->hdmarx);

      /* USART1 interrupt DeInit */
      HAL_NVIC_DisableIRQ(USART1_IRQn);
      /* USER CODE BEGIN USART1_MspDeInit 1 */

      /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
