/**
 * @file ConfigSetting.c
 * @author Vishal Keshava Murthy
 * @brief Config Setting implementation
 * @version 0.1
 * @date 2024-04-04
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "GPIO.h"
#include "ConfigSetting.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Utility table that maps GPIO levels with @ref eConfigSettingMode_t
 */
static const eConfigSettingMode_t gcConfigSettingHelper[2][2] =
{
		[GPIO_PIN_RESET][GPIO_PIN_RESET] 	= eCONFIG_SETTING_0,
		[GPIO_PIN_RESET][GPIO_PIN_SET] 		= eCONFIG_SETTING_1,
		[GPIO_PIN_SET]	[GPIO_PIN_RESET] 	= eCONFIG_SETTING_2,
		[GPIO_PIN_SET]	[GPIO_PIN_SET] 		= eCONFIG_SETTING_3,
};

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get the existing config Setting
 *
 * @return eConfigSettingMode_t current config setting
 */
eConfigSettingMode_t ConfigSetting_GetCurrentSetting()
{
	GPIO_PinState Pin1State = HAL_GPIO_ReadPin(SETTING_GPIO2_GPIO_Port, SETTING_GPIO2_Pin);
	GPIO_PinState Pin0State = HAL_GPIO_ReadPin(SETTING_GPIO1_GPIO_Port, SETTING_GPIO1_Pin);

	eConfigSettingMode_t currentConfigSetting = gcConfigSettingHelper[Pin1State][Pin0State] ;

	return currentConfigSetting;
}

///////////////////////////////////////////////////////////////////////////////
