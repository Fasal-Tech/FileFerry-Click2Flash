/**
 * @file AppFlash_API.h
 * @author Vishal Keshava Murthy
 * @brief API interface LittleFS Filesystem on external Flash
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPSTORAGE_APPFLASHFS_APPFLASH_API_H_
#define APPSTORAGE_APPFLASHFS_APPFLASH_API_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "crc.h"
#include "lfs.h"
#include "AppStorageDataStructures.h"

///////////////////////////////////////////////////////////////////////////////

#define FLASHFS_CRC_INSTANCE	(&hcrc)		/**< CRC instance used by flash module for file integrity check*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief wrapper around littleFS file structure
 * 
 */
typedef struct
{
	bool IsMounted;
	lfs_t fs;
	lfs_file_t fileHandles[eFS_MAX];
}sFlashFS_t;

///////////////////////////////////////////////////////////////////////////////
eStorageFSStatus_t FlashFs_API_Init();
eStorageFSStatus_t FlashFs_API_OpenGoldenImageFile();
eStorageFSStatus_t FlashFs_API_WriteToGoldenImageFile(const char* const pInWriteBuf, size_t bufSize);
eStorageFSStatus_t FlashFs_API_CloseGoldenImageFile();
eStorageFSStatus_t FlashFs_API_DeleteGoldenImageFile();
eStorageFSStatus_t FlashFs_API_ComputeGoldenImageFileCRC(uint8_t* const pInOutRamBuf, uint32_t RamBufSize, uint32_t* const pOutCRC);

///////////////////////////////////////////////////////////////////////////////


#endif /* APPSTORAGE_APPFLASHFS_APPFLASH_API_H_ */
