/**
 * @file W25Qxx.h
 * @author Vishal Keshava Murthy 
 * @brief W25Qxx Driver Interface
 * @version 0.1
 * @date 2023-08-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef _W25Qxx_H    /* Guard against multiple inclusion */
#define _W25Qxx_H

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include "spi.h"
#include "gpio.h"

///////////////////////////////////////////////////////////////////////////////

#define W25QXXH_SPI_HANDLE		(&hspi2)
#define W25QXXH_SPI_TIMEOUT_MS	(100)

#define W25QXXH_SPI_CS_PORT		(SPI2_NSS_GPIO_Port)
#define W25XXH_SPI_CS_PIN		(SPI2_NSS_Pin)

///////////////////////////////////////////////////////////////////////////////
    
#define _W25QXX_DEBUG           (0)
    
#define DF_MAX_NO_PAGE          65536
#define DF_PAGE_SIZE            256 
#define DF_SECTOR_SIZE          16 //16 pages in one sector = 4KB
#define DF_SBLOCK_SIZE          128 //32KB
#define DF_BBLOCK_SIZE          256 //64KB
#define DF_MAX_SECTOR           4096
#define DF_MAX_SBLOCK           512
#define DF_MAX_BBLOCK           256
    
///////////////////////////////////////////////////////////////////////////////

typedef enum
{
	W25Q10=1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
}W25QXX_ID_t;

///////////////////////////////////////////////////////////////////////////////

typedef struct{
    uint8_t txbuff[DF_PAGE_SIZE];
    uint8_t rxbuff[DF_PAGE_SIZE];
    uint8_t txbuff_len;
    uint8_t rxbuff_len;
    uint8_t rxbuff_readpos;
    uint8_t df_mode;
    
}df_dataparam;

typedef struct
{
	W25QXX_ID_t	ID;
	uint8_t		UniqID[8];
	uint16_t	PageSize;
	uint32_t	PageCount;
	uint32_t	SectorSize;
	uint32_t	SectorCount;
	uint32_t	BlockSize;
	uint32_t	BlockCount;
	uint32_t	CapacityInKiloByte;
	uint8_t		StatusRegister1;
	uint8_t		StatusRegister2;
	uint8_t		StatusRegister3;	
	uint8_t		Lock;
}w25qxx_t;

typedef int32_t eW25qxxStatus;

///////////////////////////////////////////////////////////////////////////////

//############################################################################
// in Page,Sector and block read/write functions, can put 0 to read maximum bytes 
//############################################################################
eW25qxxStatus		W25qxx_Init(void);

eW25qxxStatus		W25qxx_EraseChip(void);
eW25qxxStatus 		W25qxx_EraseSector(uint32_t SectorAddr);
eW25qxxStatus 		W25qxx_EraseBlock(uint32_t BlockAddr);

uint32_t	W25qxx_PageToSector(uint32_t PageAddress);
uint32_t	W25qxx_PageToBlock(uint32_t PageAddress);
uint32_t	W25qxx_SectorToBlock(uint32_t SectorAddress);
uint32_t	W25qxx_SectorToPage(uint32_t SectorAddress);
uint32_t	W25qxx_BlockToPage(uint32_t BlockAddress);

bool 		W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize);
bool 		W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize);
bool 		W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize);

eW25qxxStatus       W25qxx_WriteByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes);
eW25qxxStatus 		W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
eW25qxxStatus 		W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize);
eW25qxxStatus 		W25qxx_WriteBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize);

eW25qxxStatus 		W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address);
eW25qxxStatus 		W25qxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
eW25qxxStatus 		W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address,uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);
eW25qxxStatus 		W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address,uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize);
eW25qxxStatus 		W25qxx_ReadBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize);

bool W25qxx_Test();

///////////////////////////////////////////////////////////////////////////////

#endif /* _W25Qxx_H */

