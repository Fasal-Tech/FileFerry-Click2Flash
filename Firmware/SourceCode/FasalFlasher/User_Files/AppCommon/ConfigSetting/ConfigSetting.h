/**
 * @file ConfigSetting.h
 * @author Vishal Keshava Murthy
 * @brief Config Setting interface
 * @version 0.1
 * @date 2024-04-04
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPCOMMON_CONFIGSETTING_CONFIGSETTING_H_
#define APPCOMMON_CONFIGSETTING_CONFIGSETTING_H_

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Enums representing various config setting
 */
typedef enum
{
	eCONFIG_SETTING_0,
	eCONFIG_SETTING_1,
	eCONFIG_SETTING_2,
	eCONFIG_SETTING_3,
	eCONFIG_SETTING_MAX,
}eConfigSettingMode_t;

///////////////////////////////////////////////////////////////////////////////

eConfigSettingMode_t ConfigSetting_GetCurrentSetting();

///////////////////////////////////////////////////////////////////////////////


#endif /* APPCOMMON_CONFIGSETTING_CONFIGSETTING_H_ */
