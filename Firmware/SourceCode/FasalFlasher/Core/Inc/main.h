/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define FLASH_BUTTON_Pin GPIO_PIN_0
#define FLASH_BUTTON_GPIO_Port GPIOA
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define TRANSFER_MODE_Pin GPIO_PIN_0
#define TRANSFER_MODE_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define SETTING_GPIO1_Pin GPIO_PIN_10
#define SETTING_GPIO1_GPIO_Port GPIOB
#define SETTING_GPIO2_Pin GPIO_PIN_11
#define SETTING_GPIO2_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_10
#define LED_B_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_11
#define LED_R_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_12
#define LED_G_GPIO_Port GPIOC
#define LED_G1_Pin GPIO_PIN_3
#define LED_G1_GPIO_Port GPIOB
#define LED_B1_Pin GPIO_PIN_4
#define LED_B1_GPIO_Port GPIOB
#define LED_R1_Pin GPIO_PIN_5
#define LED_R1_GPIO_Port GPIOB
#define SENSOR_POWER_ENABLE_Pin GPIO_PIN_8
#define SENSOR_POWER_ENABLE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
