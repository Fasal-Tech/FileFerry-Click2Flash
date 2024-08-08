/**
 * @file AppStorage.c
 * @author Vishal Keshava Murthy
 * @brief Storage module implementation that consumes @ref AppFlashFS and @ref AppSDFS
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>

#include "SPI.h"

#include "AppStorage.h"
#include "Console.h"
#include "W25Qxx.h"
#include "AppStorageDataStructures.h"
#include "AppSD_API.h"
#include "AppFlash_API.h"

///////////////////////////////////////////////////////////////////////////////

static __attribute__ ((aligned (4))) uint8_t gRamBuf[48*1024] = {0}; 	/**< 48k Ram buffer chunks to read the file into, must be aligned to prevent alignment fault */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief utility mapping of Transfer mode @ref eTransferMode_t to GPIO state
 */
static const eTransferMode_t gcTransferModeToGPIOStatusMappingTable[eTX_MODE_MAX] =
{
		[eTX_MODE_SDCARD_TO_FLASH] = GPIO_PIN_RESET,
		[eTX_MODE_XMODEM_TO_FLASH] = GPIO_PIN_SET
};

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Set Sensor power for powering external flash board
 * @note SPI also needs to be controlled here since power on SPI pins is also powering the external board
 *
 */
void AppStorage_SetPower(bool IsEnable)
{
	if(true == IsEnable)
	{
		MX_SPI2_Init();
		HAL_GPIO_WritePin(FLASH_POWER_EN_PORT, FLASH_POWER_EN_PIN, GPIO_PIN_SET);
	}
	else
	{
		HAL_SPI_DeInit(W25QXXH_SPI_HANDLE);
		HAL_GPIO_WritePin(FLASH_POWER_EN_PORT, FLASH_POWER_EN_PIN, GPIO_PIN_RESET);
	}
}

/**
 * @brief Get current transfer mode setting
 *
 * @return eTransferMode_t
 *
 */
eTransferMode_t AppStorage_GetCurrentTransferMode()
{
	GPIO_PinState ModePinStatus = HAL_GPIO_ReadPin(TRANSFER_MODE_PORT, TRANSFE_MODE_PIN);

	return gcTransferModeToGPIOStatusMappingTable[ModePinStatus] ;
}


/**
 * @brief Transfer Golden Image from SD card to flash
 *
 * @return eAppStorageStatus_t
 */
eStorageFSStatus_t AppStorage_TransferGoldenImageFileFromSDToFlash()
{
	eStorageFSStatus_t fatFSStatus = SDFs_API_OpenGoldenImageFile();

	eStorageFSStatus_t lFSStatus = FlashFs_API_OpenGoldenImageFile();

	if((eFS_SUCCESS == fatFSStatus) && (eFS_SUCCESS == lFSStatus))
	{
		uint32_t goldenImageSizeInSDCard = 0;
		fatFSStatus |= SDFs_API_GetGoldenImageFileSize(&goldenImageSizeInSDCard);

		uint32_t fileSizeRemaining = goldenImageSizeInSDCard;
		bool IsFileTransferComplete = false;

		do
		{
			memset(gRamBuf, 0x00, sizeof(gRamBuf));
			uint32_t bytesRead = 0;

			fatFSStatus |= SDFs_API_ReadGoldenImageFile((char* const)gRamBuf, sizeof(gRamBuf), &bytesRead);

			lFSStatus |= FlashFs_API_WriteToGoldenImageFile((const char* const)gRamBuf, bytesRead);

			if(fileSizeRemaining < sizeof(gRamBuf))
			{
				IsFileTransferComplete = true;
			}
			else
			{
				fileSizeRemaining -= sizeof(gRamBuf);
			}

			Console_PrintProgressBar();

		}while(false == IsFileTransferComplete);

		SDFs_API_CloseGoldenImageFile();
		FlashFs_API_CloseGoldenImageFile();
	}
	else
	{
		SDFs_API_CloseGoldenImageFile();
		FlashFs_API_CloseGoldenImageFile();
	}

	eStorageFSStatus_t status = (fatFSStatus | lFSStatus) ;

	return status;
}

/**
 * @brief Compute and compare CRC of golden Image file stored in CRC and Flash
 *
 * @param pOutIsCRCMatching Set to true if CRC matches, False otherwise.
 * @return eAppStorageStatus_t
 */
eStorageFSStatus_t AppStorage_CompareCRCOfGoldenImageFileInSDAndFlash(bool* const pOutIsCRCMatching)
{
	assert(NULL != pOutIsCRCMatching);

	*pOutIsCRCMatching = false;

	uint32_t SDGoldenImageCRC = 0;
	uint32_t FlashGoldenImageCRC = 0;

	eStorageFSStatus_t SDFSStatus = SDFs_API_ComputeGoldenImageFileCRC(gRamBuf, sizeof(gRamBuf), &SDGoldenImageCRC);

	eStorageFSStatus_t lFSStatus = FlashFs_API_ComputeGoldenImageFileCRC(gRamBuf, sizeof(gRamBuf), &FlashGoldenImageCRC);

	if((eFS_SUCCESS == SDFSStatus) && (eFS_SUCCESS == lFSStatus))
	{
		Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> CRC of Golden Image file in SD : %X | Flash : %X", SDGoldenImageCRC, FlashGoldenImageCRC );

		if(SDGoldenImageCRC == FlashGoldenImageCRC)
		{
			*pOutIsCRCMatching = true ;
		}
		else
		{
			*pOutIsCRCMatching = false ;
		}
	}

	eStorageFSStatus_t status = (SDFSStatus | lFSStatus) ;

	return status;
}


