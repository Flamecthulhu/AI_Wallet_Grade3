/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EPD_MISO_Pin GPIO_PIN_2
#define EPD_MISO_GPIO_Port GPIOC
#define EPD_MOSI_Pin GPIO_PIN_3
#define EPD_MOSI_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define EPD_SCK_Pin GPIO_PIN_10
#define EPD_SCK_GPIO_Port GPIOB
#define EPD_CS_Pin GPIO_PIN_12
#define EPD_CS_GPIO_Port GPIOB
#define EPD_BUSY_Pin GPIO_PIN_5
#define EPD_BUSY_GPIO_Port GPIOD
#define EPD_RST_Pin GPIO_PIN_6
#define EPD_RST_GPIO_Port GPIOD
#define EPD_DC_Pin GPIO_PIN_7
#define EPD_DC_GPIO_Port GPIOD
#define BLE_RX_Pin GPIO_PIN_3
#define BLE_RX_GPIO_Port GPIOB
#define BLE_TX_Pin GPIO_PIN_4
#define BLE_TX_GPIO_Port GPIOB
#define GPS_RX_Pin GPIO_PIN_5
#define GPS_RX_GPIO_Port GPIOB
#define GPS_TX_Pin GPIO_PIN_6
#define GPS_TX_GPIO_Port GPIOB
#define WF_RX_Pin GPIO_PIN_8
#define WF_RX_GPIO_Port GPIOB
#define WF_TX_Pin GPIO_PIN_9
#define WF_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
