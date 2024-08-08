/**
 * @file AppSD_API.c
 * @author Vishal Keshava Murthy
 * @brief API implementation of FATFS Filesystem on SD-Card
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>

#include "AppSD_API.h"
#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Utility table to convert user file-system modes to FatFS file system modes
 * 
 */
static const uint8_t gcOpModeToFatFSModeConverterTable[eFS_MODE_MAX] =
{
		[eFS_READONLY]			= (FA_READ),
		[eFS_WRITEONLY]			= (FA_WRITE),
		[eFS_READ_WRITE]		= (FA_READ  | FA_WRITE),
		[eFS_READ_CREATE]		= (FA_READ  | FA_OPEN_ALWAYS),
		[eFS_WRITE_CREATE]		= (FA_WRITE | FA_OPEN_ALWAYS),
		[eFS_WRITE_APPEND]		= (FA_WRITE | FA_OPEN_ALWAYS),
		[eFS_READ_WRITE_CREATE]	= (FA_READ  | FA_WRITE | FA_OPEN_ALWAYS),
};

/**
 * @brief Map Name enums to File name strings
 *
 */
static const char* const gcFilesNamesTable[eFS_MAX] =
{
		[eFS_GOLDEN_IMAGE] 	= "fallback.txt",
		[eFS_FILE_2] 		= "file2.txt"
};

///////////////////////////////////////////////////////////////////////////////

static sSDFS_t gSDCard = {.IsMounted = false};		/**< Global SD-card instance */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get storage media instance
 *
 * @return sSDFS_t*
 */
static sSDFS_t* SDFs_GetInstance()
{
	return &gSDCard;
}

/**
 * @brief Initialize FatFS file-system
 *
 * @param pMe FatFS wrapper instance
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t SDFs_Init(sSDFS_t* const pMe)
{
	assert(NULL != pMe);
	eStorageFSStatus_t status = eFS_ERROR;
	FRESULT fRes = f_mount(&(pMe->fs),"",1);
	if(FR_OK == fRes)
	{
		pMe->IsMounted = true;
		status = eFS_SUCCESS;
	}
	return status;
}

/**
 * @brief Initialize FatFS file-system
 *
 * @param pMe FatFS wrapper instance
 * @return eStorageFSStatus_t
 */
__attribute__((unused)) static eStorageFSStatus_t SDFs_DeInit(sSDFS_t* const pMe)
{
	assert(NULL != pMe);
	eStorageFSStatus_t status = eFS_ERROR;

	FRESULT fRes = f_mount(&(pMe->fs),"",0);
	if(FR_OK == fRes)
	{
		pMe->IsMounted = false;
		status = eFS_SUCCESS;
	}
	return status;
}

/**
 * @brief Open a file in FatFS
 * 
 * @param pMe FatFS wrapper instance
 * @param fileEnum File to be opened 
 * @param modeEnum Mode to open file In
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t SDFs_OpenFileRaw(sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, eStorageFSOperationModes_t modeEnum)
{
	assert(NULL != pMe);
	assert( fileEnum < eFS_MAX  );
	assert(modeEnum < eFS_MODE_MAX);

	uint8_t mode = gcOpModeToFatFSModeConverterTable[modeEnum];

	FRESULT fRes = f_open(&(pMe->fileHandles[fileEnum]), gcFilesNamesTable[fileEnum], mode );

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief Close a file in FatFS
 * 
 * @param pMe FatFS wrapper instance
 * @param fileEnum file to be closed
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t SDFs_CloseFileRaw(sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum)
{
	assert(NULL != pMe);
	assert( fileEnum < eFS_MAX  );

	FRESULT fRes = f_close(&(pMe->fileHandles[fileEnum]));

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief read from a file in FatFS
 *
 * @param pMe FatFS wrapper instance
 * @param fileEnum File to be read from
 * @param pOutReadBuf read data will be saved here
 * @param bytesToRead number of bytes to read
 * @param pOutBytesRead number of bytes read is saved here
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t SDFs_ReadFileRaw(sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, char* const pOutReadBuf, uint32_t bytesToRead, uint32_t* const pOutBytesRead)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX );
	assert(NULL != pOutBytesRead);

	FRESULT fRes = f_read(&(pMe->fileHandles[fileEnum]), pOutReadBuf, bytesToRead, (unsigned int*)pOutBytesRead);

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief Write to a file in FatFS
 * 
 * @param pMe FatFS wrapper instance
 * @param fileEnum File to be written into 
 * @param IsAppend Pass true to write to file in append mode, false to write at the beginning
 * @param pInWriteBuf data to be written is passed here
 * @param bufSize size of buffer passed for writing
 * @return eStorageFSStatus_t 
 */
__attribute__((unused)) static eStorageFSStatus_t SDFs_WriteFileRaw(sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, bool IsAppend, const char* const pInWriteBuf, size_t bufSize)
{
	assert(NULL != pMe);
	assert( fileEnum < eFS_MAX  );
	assert(NULL != pInWriteBuf);

	FRESULT fRes = FR_OK;
	if(true == IsAppend)
	{
		fRes |= f_lseek(&(pMe->fileHandles[fileEnum]), f_size(&(pMe->fileHandles[fileEnum])));
	}

	size_t bytesWritten = 0;
	fRes |= f_write(&(pMe->fileHandles[fileEnum]), pInWriteBuf, bufSize, &bytesWritten);

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief Delete File
 *
 * @param pMe FatFS wrapper instance
 * @param fileEnum File to be deleted
 * @return eStorageFSStatus_t
 */
__attribute__((unused)) static eStorageFSStatus_t SDFs_DeleteFile(const sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX);

	FRESULT fRes = f_unlink(gcFilesNamesTable[fileEnum]);

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief compute CRC of requested file
 *
 * @param pMe FatFS wrapper instance
 * @param fileEnum File whose CRC needs to be computed
 * @param pInOutRamBuf Ram buffer that will be used as temporary storage while computing CRC
 * @param RamBufSize ram buffer size passed
 * @param pOutCRC computed CRC will be saved here
 *
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t SDFs_ComputeFileCRC(sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, uint8_t* const pInOutRamBuf, uint32_t RamBufSize,  uint32_t* const pOutCRC)
{
	assert(NULL != pMe);
	assert(NULL != pOutCRC);
	assert(NULL != pInOutRamBuf);
	assert(fileEnum < eFS_MAX);

	eStorageFSStatus_t status = SDFs_OpenFileRaw(pMe, fileEnum, eFS_READONLY);

	if(eFS_SUCCESS == status)
	{
		__HAL_RCC_CRC_CLK_ENABLE();
		__HAL_LOCK(SDFS_CRC_INSTANCE);
		__HAL_CRC_DR_RESET(SDFS_CRC_INSTANCE);
		__HAL_UNLOCK(SDFS_CRC_INSTANCE);

		memset(pInOutRamBuf, 0, RamBufSize);

		uint32_t numBytesRead = 0;

		do
		{
			SDFs_ReadFileRaw(pMe, fileEnum, (char* const)pInOutRamBuf, RamBufSize, &numBytesRead);

			for(int i=0; i<numBytesRead; i=(i+4))
			{
				uint32_t crcIntermediete = (pInOutRamBuf[i]<<24) | (pInOutRamBuf[i+1]<<16) | (pInOutRamBuf[i+2]<<8) | (pInOutRamBuf[i+3]) ;
				(*pOutCRC) = HAL_CRC_Accumulate(SDFS_CRC_INSTANCE, &crcIntermediete, 1);
			}

		}while(RamBufSize == numBytesRead);

		SDFs_CloseFileRaw(pMe, fileEnum);

	}

	return status;
}


/**
 * @brief Get size of file in FatFS
 * 
 * @param pMe FatFS wrapper instance
 * @param fileEnum file whose size needs to be determined 
 * @param pOutFileSizeInBytes size of the file computed is saved here
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t SDFs_GetFileSizeRaw(const sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, uint32_t* const pOutFileSizeInBytes)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX );
	assert(NULL != pOutFileSizeInBytes);

	(*pOutFileSizeInBytes) = f_size(&(pMe->fileHandles[fileEnum]));

	return eFS_SUCCESS;
}


/**
 * @brief Determine if a file is present in FatFS
 *
 * @param pMe FatFS wrapper instance
 * @param fileEnum file to be checked
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t SDFs_GetFilePresent(sSDFS_t* const pMe, eStorageFileNamesEnums_t fileEnum)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX);

	eStorageFSStatus_t status = SDFs_OpenFileRaw(pMe, fileEnum, eFS_READONLY);

	status |= SDFs_CloseFileRaw(pMe, fileEnum);

	return (status);

}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize FatFS wrapper
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t SDFs_API_Init()
{
	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_Init(pMe);

	return status;
}

/**
 * @brief Check if file exists
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t SDFs_API_GetGoldenFileStatus()
{
	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_GetFilePresent(pMe, eFS_GOLDEN_IMAGE);

	return status;
}


/**
 * @brief Open Golden Image file
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t SDFs_API_OpenGoldenImageFile()
{
	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_OpenFileRaw(pMe, eFS_GOLDEN_IMAGE, eFS_READONLY );

	return status;
}

/**
 * @brief Get size of golden image
 * 
 * @param pOutFileSizeInBytes size of the golden image is saved here 
 * @return eStorageFSStatus_t 
 */
eStorageFSStatus_t SDFs_API_GetGoldenImageFileSize(uint32_t* pOutFileSizeInBytes)
{
	assert(NULL != pOutFileSizeInBytes);

	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_GetFileSizeRaw(pMe, eFS_GOLDEN_IMAGE, pOutFileSizeInBytes );

	return status;
}

/**
 * @brief Read golden image file 
 * 
 * @param pOutReadBuf contents of read file are saved here 
 * @param bytesToRead bytes to read from file 
 * @param pOutBytesRead bytes read from file
 * @return eStorageFSStatus_t 
 */
eStorageFSStatus_t SDFs_API_ReadGoldenImageFile(char* const pOutReadBuf, uint32_t bytesToRead, uint32_t* const pOutBytesRead)
{
	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_ReadFileRaw(pMe, eFS_GOLDEN_IMAGE, pOutReadBuf, bytesToRead, pOutBytesRead );

	return status;
}

/**
 * @brief Close golden Image file
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t SDFs_API_CloseGoldenImageFile()
{
	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_CloseFileRaw(pMe, eFS_GOLDEN_IMAGE);

	return status;
}

/**
 * @brief compute CRC of golden Image file
 *
 * @param pInOutRamBuf Ram buffer that will be used as temporary storage while computing CRC
 * @param RamBufSize ram buffer size passed
 * @param pOutCRC computed CRC will be saved here
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t SDFs_API_ComputeGoldenImageFileCRC(uint8_t* const pInOutRamBuf, uint32_t RamBufSize, uint32_t* const pOutCRC)
{
	assert(NULL != pInOutRamBuf);
	assert(NULL != pOutCRC);

	sSDFS_t* pMe = SDFs_GetInstance();

	eStorageFSStatus_t status = SDFs_ComputeFileCRC(pMe, eFS_GOLDEN_IMAGE, pInOutRamBuf, RamBufSize, pOutCRC);

	return status;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_TESTS_DEFINITIONS

bool SDFs_API_IsLLTestPass()
{
	return false;
}

#endif
