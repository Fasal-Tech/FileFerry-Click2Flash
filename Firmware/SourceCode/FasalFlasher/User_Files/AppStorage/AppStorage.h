/**
 * @file AppStorage.h
 * @author Vishal Keshava Murthy
 * @brief App storage interface 
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef APPSTORAGE_APPSTORAGE_H_
#define APPSTORAGE_APPSTORAGE_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "AppStorageDataStructures.h"

///////////////////////////////////////////////////////////////////////////////

#define FLASH_POWER_EN_PORT	(SENSOR_POWER_ENABLE_GPIO_Port)		/**< Port of GPIO that controls power to external board*/
#define FLASH_POWER_EN_PIN	(SENSOR_POWER_ENABLE_Pin)			/**< Pin of GPIO that controls power to external board*/

#define TRANSFER_MODE_PORT	(TRANSFER_MODE_GPIO_Port)			/**< Port of GPIO that determines the transfer mode of operation*/
#define TRANSFE_MODE_PIN	(TRANSFER_MODE_Pin)					/**< Pin of GPIO that determines the transfer mode of operation*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Enumerations for storage mediums supported
 *
 */
typedef enum
{
	eTX_MODE_SDCARD_TO_FLASH,
	eTX_MODE_XMODEM_TO_FLASH,
	eTX_MODE_MAX
}eTransferMode_t;

///////////////////////////////////////////////////////////////////////////////

void AppStorage_SetPower(bool IsEnable);
eStorageFSStatus_t AppStorage_TransferGoldenImageFileFromSDToFlash();
eStorageFSStatus_t AppStorage_CompareCRCOfGoldenImageFileInSDAndFlash(bool* const pOutIsCRCMatching);
eTransferMode_t AppStorage_GetCurrentTransferMode();

///////////////////////////////////////////////////////////////////////////////

#endif /* APPSTORAGE_APPSTORAGE_H_ */
