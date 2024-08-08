/**
 * @file PushButton.c
 * @author Vishal Keshava Murthy
 * @brief Push Button Implementation
 * @version 0.1
 * @date 2024-04-04
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"

#include "PushButton.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Check if Push Button is pressed
 * @return true if button is pressed, false if not during function invocation
 */
bool PushButton_IsFlashButtonPressed()
{
	GPIO_PinState pinstate = HAL_GPIO_ReadPin(FLASH_BUTTON_GPIO_Port, FLASH_BUTTON_Pin);

	return (GPIO_PIN_RESET == pinstate);
}

///////////////////////////////////////////////////////////////////////////////
