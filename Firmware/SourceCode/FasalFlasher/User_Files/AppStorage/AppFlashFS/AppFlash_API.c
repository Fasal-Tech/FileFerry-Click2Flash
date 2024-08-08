/**
 * @file AppFlash_API.c
 * @author Vishal Keshava Murthy
 * @brief API implementation of LittleFS Filesystem on external Flash
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>

#include "AppFlash_API.h"
#include "W25Qxx.h"
#include "LittleFS_Wrapper.h"
#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief utility table that converts user file modes to littleFS file modes
 * 
 */
static const int gcOpModeToLittleFSModeConverterTable[eFS_MODE_MAX] =
{
		[eFS_READONLY]			= (LFS_O_RDONLY),
		[eFS_WRITEONLY]			= (LFS_O_WRONLY),
		[eFS_READ_WRITE]		= (LFS_O_RDWR),
		[eFS_READ_CREATE]		= (LFS_O_RDONLY | LFS_O_CREAT),
		[eFS_WRITE_CREATE]		= (LFS_O_WRONLY | LFS_O_CREAT),
		[eFS_WRITE_APPEND]		= (LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND),
		[eFS_READ_WRITE_CREATE]	= (LFS_O_RDWR   | LFS_O_CREAT),
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

static sFlashFS_t gsFlash = {.IsMounted = false} ;	/**< littleFS wrapper structure instance*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get FlashFS file instance
 * 
 * @return sFlashFS_t* 
 */
static sFlashFS_t* FlashFS_GetInstance()
{
	return &gsFlash;
}

/**
 * @brief Initialize lfs
 * 
 * @param pMe lfs wrapper instance
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t FlashFS_Init(sFlashFS_t* const pMe)
{
	assert(NULL != pMe);

	eStorageFSStatus_t status = eFS_ERROR;

    W25qxx_Init();

    int fRes = lfsWrapper_Init((lfs_t*)&(pMe->fs));
	if(0 == fRes)
	{
		pMe->IsMounted = true;
		status = eFS_SUCCESS;
	}
	return status;
}

/**
 * @brief Deinitialize LFS file system
 * 
 * @param pMe lfs wrapper instance
 * @return eStorageFSStatus_t 
 */
__attribute__((unused)) static eStorageFSStatus_t FlashFS_DeInit(sFlashFS_t* const pMe)
{
	assert(NULL != pMe);

	eStorageFSStatus_t status = eFS_ERROR;

	int fRes = lfs_unmount((lfs_t*)&(pMe->fs));
	if(0 == fRes)
	{
		pMe->IsMounted = false;
		status = eFS_SUCCESS;
	}
	return status;
}

/**
 * @brief Open a file in lfs
 * 
 * @param pMe lfs wrapper instance
 * @param fileEnum File to be opened 
 * @param modeEnum Mode to open file In
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t FlashFs_OpenFileRaw(sFlashFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, eStorageFSOperationModes_t modeEnum)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX);

	int mode = gcOpModeToLittleFSModeConverterTable[modeEnum];

	int fRes = lfs_file_open((lfs_t*)&(pMe->fs), &(pMe->fileHandles[fileEnum]), gcFilesNamesTable[fileEnum], mode );
	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief Close a file in lfs
 * 
 * @param pMe lfs wrapper instance
 * @param fileEnum file to be closed
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t FlashFs_CloseFileRaw(sFlashFS_t* const pMe, eStorageFileNamesEnums_t fileEnum)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX);

	int fRes = lfs_file_close((lfs_t*)&(pMe->fs), &(pMe->fileHandles[fileEnum]));

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief read from a file in FlashFS
 *
 * @param pMe FlashFS wrapper instance
 * @param fileEnum File to be read from
 * @param pOutReadBuf read data will be saved here
 * @param bytesToRead number of bytes to read
 * @param pOutBytesRead number of bytes read is saved here
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t FlashFs_ReadFileRaw(sFlashFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, char* const pOutReadBuf, uint32_t bytesToRead, int32_t* const pOutBytesRead)
{
	assert(NULL != pMe);
	assert(NULL != pOutBytesRead);
	assert(fileEnum < eFS_MAX );

	*pOutBytesRead = lfs_file_read((lfs_t*)&(pMe->fs), &(pMe->fileHandles[fileEnum]), pOutReadBuf, bytesToRead);

	return eFS_SUCCESS;
}

/**
 * @brief Write to a file in LFS
 * 
 * @param pMe lfs wrapper instance
 * @param fileEnum File to be written into 
 * @param IsAppend Unused Kept for API consistency
 * @param pInWriteBuf data to be written is passed here
 * @param bufSize size of buffer passed for writing
 * @return eStorageFSStatus_t 
 */
static eStorageFSStatus_t FlashFs_WriteFileRaw(sFlashFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, bool IsAppend, const char* const pInWriteBuf, size_t bufSize)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX);
	assert(NULL != pInWriteBuf);

	UNUSED(IsAppend); /**< @warning IsAppend Kept for API consistency, File should be opened in Append mode if Appending is needed */

	size_t bytesWritten = lfs_file_write((lfs_t*)&(pMe->fs), &(pMe->fileHandles[fileEnum]), pInWriteBuf, bufSize);

	eStorageFSStatus_t status = (bytesWritten == bufSize)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief Delete File
 *
 * @param pMe lfs wrapper instance
 * @param fileEnum File to be deleted
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t FlashFs_DeleteFile(const sFlashFS_t* const pMe, eStorageFileNamesEnums_t fileEnum)
{
	assert(NULL != pMe);
	assert(fileEnum < eFS_MAX);

	int fRes = lfs_remove((lfs_t*)&(pMe->fs), gcFilesNamesTable[fileEnum]);

	eStorageFSStatus_t status = (0 == fRes)? eFS_SUCCESS: eFS_ERROR;

	return status;
}

/**
 * @brief compute CRC of requested file
 *
 * @param pMe lfs wrapper instance
 * @param fileEnum File whose CRC needs to be computed
 * @param pInOutRamBuf Ram buffer that will be used as temporary storage while computing CRC
 * @param RamBufSize ram buffer size passed
 * @param pOutCRC computed CRC will be saved here
 *
 * @return eStorageFSStatus_t
 */
static eStorageFSStatus_t FlashFs_ComputeFileCRC(sFlashFS_t* const pMe, eStorageFileNamesEnums_t fileEnum, uint8_t* const pInOutRamBuf, uint32_t RamBufSize,  uint32_t* const pOutCRC)
{
	assert(NULL != pMe);
	assert(NULL != pOutCRC);
	assert(NULL != pInOutRamBuf);
	assert(fileEnum < eFS_MAX);

	eStorageFSStatus_t status = FlashFs_OpenFileRaw(pMe, fileEnum, eFS_READONLY);

	if(eFS_SUCCESS == status)
	{
		__HAL_RCC_CRC_CLK_ENABLE();
		__HAL_LOCK(FLASHFS_CRC_INSTANCE);
		__HAL_CRC_DR_RESET(FLASHFS_CRC_INSTANCE);
		__HAL_UNLOCK(FLASHFS_CRC_INSTANCE);

		memset(pInOutRamBuf, 0, RamBufSize);

		int32_t numBytesRead = 0;

		do
		{
			FlashFs_ReadFileRaw(pMe, fileEnum, (char* const)pInOutRamBuf, RamBufSize, &numBytesRead);

			for(int i=0; i<numBytesRead; i=(i+4))
			{
				uint32_t crcIntermediete = (pInOutRamBuf[i]<<24) | (pInOutRamBuf[i+1]<<16) | (pInOutRamBuf[i+2]<<8) | (pInOutRamBuf[i+3]) ;
				(*pOutCRC) = HAL_CRC_Accumulate(FLASHFS_CRC_INSTANCE, &crcIntermediete, 1);
			}

		}while(RamBufSize == numBytesRead);

		FlashFs_CloseFileRaw(pMe, fileEnum);

	}

	return status;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief API to initialize lfs
 * 
 * @return eStorageFSStatus_t 
 */
eStorageFSStatus_t FlashFs_API_Init()
{
	sFlashFS_t* pMe = FlashFS_GetInstance();

	eStorageFSStatus_t status = FlashFS_Init(pMe);

	return status;
}

/**
 * @brief Open Golden Image file
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t FlashFs_API_OpenGoldenImageFile()
{
	sFlashFS_t* pMe = FlashFS_GetInstance();

	eStorageFSStatus_t status = FlashFs_OpenFileRaw(pMe, eFS_GOLDEN_IMAGE, eFS_WRITE_APPEND);

	return status;
}

/**
 * @brief Write to Golden Image file
 *
 * @note @ref AppStorage_API_OpenGoldenFile must be invoked prior to using this function
 *
 * @param pInWriteBuf Contents to be written to Golden file is passed here
 * @param bufSize write buffer size
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t FlashFs_API_WriteToGoldenImageFile(const char* const pInWriteBuf, size_t bufSize)
{

	sFlashFS_t* pMe = FlashFS_GetInstance();

	eStorageFSStatus_t status = FlashFs_WriteFileRaw(pMe, eFS_GOLDEN_IMAGE, true, pInWriteBuf, bufSize );

	return status;
}

/**
 * @brief Close golden Image file
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t FlashFs_API_CloseGoldenImageFile()
{
	sFlashFS_t* pMe = FlashFS_GetInstance();

	eStorageFSStatus_t status = FlashFs_CloseFileRaw(pMe, eFS_GOLDEN_IMAGE);

	return status;
}

/**
 * @brief Close golden Image file
 *
 * @return eStorageFSStatus_t
 */
eStorageFSStatus_t FlashFs_API_DeleteGoldenImageFile()
{
	sFlashFS_t* pMe = FlashFS_GetInstance();

	eStorageFSStatus_t status = FlashFs_DeleteFile(pMe, eFS_GOLDEN_IMAGE);

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
eStorageFSStatus_t FlashFs_API_ComputeGoldenImageFileCRC(uint8_t* const pInOutRamBuf, uint32_t RamBufSize, uint32_t* const pOutCRC)
{
	assert(NULL != pInOutRamBuf);
	assert(NULL != pOutCRC);

	sFlashFS_t* pMe = FlashFS_GetInstance();

	eStorageFSStatus_t status = FlashFs_ComputeFileCRC(pMe, eFS_GOLDEN_IMAGE, pInOutRamBuf, RamBufSize, pOutCRC);

	return status;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_TESTS_DEFINITIONS

bool FlashFs_API_IsLLTestPass()
{
	return false;
}

#endif







