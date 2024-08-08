/**
 * @file AppSD_API.h
 * @author Vishal Keshava Murthy
 * @brief API Interface of FATFS Filesystem on SD-Card
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPSTORAGE_APPSDFS_APPSD_API_H_
#define APPSTORAGE_APPSDFS_APPSD_API_H_

#include <stdbool.h>
#include "crc.h"
#include "ff.h"
#include "AppStorageDataStructures.h"

///////////////////////////////////////////////////////////////////////////////

#define SDFS_CRC_INSTANCE	(&hcrc)	/**< CRC instance used by SD-Card module for file integrity check*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Wrapper around FatFS file System
 * 
 */
typedef struct
{
	bool IsMounted;
	FATFS fs;
	FIL fileHandles[eFS_MAX];
}sSDFS_t;

///////////////////////////////////////////////////////////////////////////////

eStorageFSStatus_t SDFs_API_Init();
eStorageFSStatus_t SDFs_API_GetGoldenFileStatus();
eStorageFSStatus_t SDFs_API_OpenGoldenImageFile();
eStorageFSStatus_t SDFs_API_GetGoldenImageFileSize(uint32_t* pOutFileSizeInBytes);
eStorageFSStatus_t SDFs_API_ReadGoldenImageFile(char* const pOutReadBuf, uint32_t bytesToRead, uint32_t* const pOutBytesRead);
eStorageFSStatus_t SDFs_API_CloseGoldenImageFile();
eStorageFSStatus_t SDFs_API_ComputeGoldenImageFileCRC(uint8_t* const pInOutRamBuf, uint32_t RamBufSize, uint32_t* const pOutCRC);

///////////////////////////////////////////////////////////////////////////////


#endif /* APPSTORAGE_APPSDFS_APPSD_API_H_ */
