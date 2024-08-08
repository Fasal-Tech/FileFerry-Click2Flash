/**
 * @file W25Qxx.c
 * @author Vishal Keshava Murthy 
 * @brief W25Qxx Driver Implementation
 * @version 0.1
 * @date 2023-08-05
 * @copyright Copyright (c) 2023
 * @todo Timeouts to be added to all functions
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <stddef.h>                     
#include <stdbool.h>                    
#include <stdlib.h>                     
#include <stdio.h>
#include <string.h>

#include "W25qxx.h"
#include "DebugPrint.h"

#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

#define W25QXX_DUMMY_BYTE       (0xA5)
#define	W25qxx_Delay(delay)		HAL_Delay(delay)

///////////////////////////////////////////////////////////////////////////////

static w25qxx_t	gW25qxxDev; /**< Global Device instance*/

///////////////////////////////////////////////////////////////////////////////


void FLASH_SS_Clear()
{
	HAL_GPIO_WritePin(W25QXXH_SPI_CS_PORT, W25XXH_SPI_CS_PIN, GPIO_PIN_RESET);
}

void FLASH_SS_Set()
{
	HAL_GPIO_WritePin(W25QXXH_SPI_CS_PORT, W25XXH_SPI_CS_PIN, GPIO_PIN_SET);
}

uint8_t	W25qxx_Spi(uint8_t	Data)
{
	uint8_t ret;
	HAL_SPI_TransmitReceive(W25QXXH_SPI_HANDLE, &Data, &ret, 1, W25QXXH_SPI_TIMEOUT_MS);
	return ret;
}

uint32_t W25qxx_ReadID(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
    FLASH_SS_Clear();// CS to low
    W25qxx_Spi(0x9F);
    Temp0 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    Temp1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    Temp2 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    FLASH_SS_Set();
    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return Temp;
}

void W25qxx_ReadUniqID(void)
{
    FLASH_SS_Clear();
    W25qxx_Spi(0x4B);
	for(uint8_t	i=0;i<4;i++)
	{
		W25qxx_Spi(W25QXX_DUMMY_BYTE);
	}

	for(uint8_t	i=0;i<8;i++)
	{
		gW25qxxDev.UniqID[i] = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	}
  FLASH_SS_Set();
}

void W25qxx_WriteEnable(void)
{
    FLASH_SS_Clear();
    W25qxx_Spi(0x06);
    FLASH_SS_Set();
	W25qxx_Delay(1);
}

void W25qxx_WriteDisable(void)
{
    FLASH_SS_Clear();
    W25qxx_Spi(0x04);
    FLASH_SS_Set();
	W25qxx_Delay(1);
}


uint8_t W25qxx_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
    uint8_t	status=0;
    FLASH_SS_Clear();

	switch(SelectStatusRegister_1_2_3)
	{
	case 1:
		W25qxx_Spi(0x05);
		status=W25qxx_Spi(W25QXX_DUMMY_BYTE);	
		gW25qxxDev.StatusRegister1 = status;
		break;

	case 2:
		W25qxx_Spi(0x35);
		status=W25qxx_Spi(W25QXX_DUMMY_BYTE);	
		gW25qxxDev.StatusRegister2 = status;
		break;

	case 3:
		W25qxx_Spi(0x15);
		status=W25qxx_Spi(W25QXX_DUMMY_BYTE);	
		gW25qxxDev.StatusRegister3 = status;
		break;

	default:
		break;
	}

    FLASH_SS_Set();
    return status;
}


void W25qxx_WriteStatusRegister(uint8_t	SelectStatusRegister_1_2_3, uint8_t Data)
{
    FLASH_SS_Clear();

	switch(SelectStatusRegister_1_2_3)
	{
	case 1:
		W25qxx_Spi(0x01);
		gW25qxxDev.StatusRegister1 = Data;
		break;

	case 2:
		W25qxx_Spi(0x31);
		gW25qxxDev.StatusRegister2 = Data;
		break;

	case 3:
		W25qxx_Spi(0x11);
		gW25qxxDev.StatusRegister3 = Data;
		break;

	default:
		break;
	}

	W25qxx_Spi(Data);
    FLASH_SS_Set();
}


void W25qxx_WaitForWriteEnd(void)
{
    W25qxx_Delay(1);
    FLASH_SS_Clear();
    W25qxx_Spi(0x05);
    do
    {
        gW25qxxDev.StatusRegister1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		W25qxx_Delay(1);
    }
    while ((gW25qxxDev.StatusRegister1 & 0x01) == 0x01);
    FLASH_SS_Set();
}


eW25qxxStatus W25qxx_Init(void)
{
    gW25qxxDev.Lock=1;

	while (HAL_GetTick() < 100)
	{
		W25qxx_Delay(1);
	}

    FLASH_SS_Set();
    W25qxx_Delay(100);
    uint32_t	id;
    #if (_W25QXX_DEBUG==1)
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Init Begin...\r\n");
    #endif

    id=W25qxx_ReadID();

    #if (_W25QXX_DEBUG==1)
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ID:0x%X\r\n", id);
    #endif
    switch(id & 0x000000FF)
    {
		case 0x20:	// 	w25q512
			gW25qxxDev.ID=W25Q512;
			gW25qxxDev.BlockCount=1024;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q512\r\n");
			#endif
		break;
		case 0x19:	// 	w25q256
			gW25qxxDev.ID=W25Q256;
			gW25qxxDev.BlockCount=512;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q256\r\n");
			#endif
		break;
		case 0x18:	// 	w25q128
			gW25qxxDev.ID=W25Q128;
			gW25qxxDev.BlockCount=256;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q128\r\n");
			#endif
		break;
		case 0x17:	//	w25q64
			gW25qxxDev.ID=W25Q64;
			gW25qxxDev.BlockCount=128;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q64\r\n");
			#endif
		break;
		case 0x16:	//	w25q32
			gW25qxxDev.ID=W25Q32;
			gW25qxxDev.BlockCount=64;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q32\r\n");
			#endif
		break;
		case 0x15:	//	w25q16
			gW25qxxDev.ID=W25Q16;
			gW25qxxDev.BlockCount=32;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q16\r\n");
			#endif
		break;
		case 0x14:	//	w25q80
			gW25qxxDev.ID=W25Q80;
			gW25qxxDev.BlockCount=16;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q80\r\n");
			#endif
		break;
		case 0x13:	//	w25q40
			gW25qxxDev.ID=W25Q40;
			gW25qxxDev.BlockCount=8;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q40\r\n");
			#endif
		break;
		case 0x12:	//	w25q20
			gW25qxxDev.ID=W25Q20;
			gW25qxxDev.BlockCount=4;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q20\r\n");
			#endif
		break;
		case 0x11:	//	w25q10
			gW25qxxDev.ID=W25Q10;
			gW25qxxDev.BlockCount=2;
			#if (_W25QXX_DEBUG==1)
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Chip: w25q10\r\n");
			#endif
		break;
		default:
				#if (_W25QXX_DEBUG==1)
				DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Unknown ID\r\n");
				#endif
			gW25qxxDev.Lock=0;	
			return false;
				
	}		
	gW25qxxDev.PageSize=256;
	gW25qxxDev.SectorSize=0x1000;
	gW25qxxDev.SectorCount=gW25qxxDev.BlockCount*16;
	gW25qxxDev.PageCount=(gW25qxxDev.SectorCount*gW25qxxDev.SectorSize)/gW25qxxDev.PageSize;
	gW25qxxDev.BlockSize=gW25qxxDev.SectorSize*16;
	gW25qxxDev.CapacityInKiloByte=(gW25qxxDev.SectorCount*gW25qxxDev.SectorSize)/1024;
	W25qxx_ReadUniqID();
	W25qxx_ReadStatusRegister(1);
	W25qxx_ReadStatusRegister(2);
	W25qxx_ReadStatusRegister(3);

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Page Size: %d Bytes\r\n",gW25qxxDev.PageSize);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Page Count: %d\r\n",gW25qxxDev.PageCount);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Sector Size: %d Bytes\r\n",gW25qxxDev.SectorSize);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Sector Count: %d\r\n",gW25qxxDev.SectorCount);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Block Size: %d Bytes\r\n",gW25qxxDev.BlockSize);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Block Count: %d\r\n",gW25qxxDev.BlockCount);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Capacity: %d KiloBytes\r\n",gW25qxxDev.CapacityInKiloByte);
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev Init Done\r\n");
	#endif

	gW25qxxDev.Lock=0;	
	return 0;
}	


eW25qxxStatus W25qxx_EraseChip(void)
{
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "W25qxx_EraseChip initiated please wait\n\r");
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseChip Begin...\r\n");
	#endif

	W25qxx_WriteEnable();
    FLASH_SS_Clear();
	
    W25qxx_Spi(0xC7);
    FLASH_SS_Set();
 
	W25qxx_WaitForWriteEnd();

	#if (_W25QXX_DEBUG==1)
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseBlock done\n\r");
	#endif

	W25qxx_Delay(10);
	gW25qxxDev.Lock=0;	
    
    return 0;
}


eW25qxxStatus W25qxx_EraseSector(uint32_t SectorAddr)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseSector %d Begin...\r\n",SectorAddr);
	#endif

	W25qxx_WaitForWriteEnd();
	SectorAddr = SectorAddr * gW25qxxDev.SectorSize;
	W25qxx_WriteEnable();
	FLASH_SS_Clear();

	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0x21);
		W25qxx_Spi((SectorAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0x20);
	}

	W25qxx_Spi((SectorAddr & 0xFF0000) >> 16);
	W25qxx_Spi((SectorAddr & 0xFF00) >> 8);
	W25qxx_Spi(SectorAddr & 0xFF);
	FLASH_SS_Set();
	W25qxx_WaitForWriteEnd();

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseSector done\n\r");
	#endif

	W25qxx_Delay(1);
	gW25qxxDev.Lock=0;

	return 0;
}
 
eW25qxxStatus W25qxx_EraseBlock(uint32_t BlockAddr)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}

	gW25qxxDev.Lock=1;

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseBlock %d Begin...\r\n",BlockAddr);
	W25qxx_Delay(100);
	#endif

	W25qxx_WaitForWriteEnd();
	BlockAddr = BlockAddr * gW25qxxDev.SectorSize*16;
    W25qxx_WriteEnable();
    FLASH_SS_Clear();
  
	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0xDC);
		W25qxx_Spi((BlockAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0xD8);
	}

    W25qxx_Spi((BlockAddr & 0xFF0000) >> 16);
    W25qxx_Spi((BlockAddr & 0xFF00) >> 8);
    W25qxx_Spi(BlockAddr & 0xFF);
    FLASH_SS_Set();	
    W25qxx_WaitForWriteEnd();

    #if (_W25QXX_DEBUG==1)
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseBlock done\n\r");
    W25qxx_Delay(100);
    #endif

    W25qxx_Delay(1);
    gW25qxxDev.Lock=0;
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev EraseBlock done\n\r");
    
    return 0;
}
 
uint32_t W25qxx_PageToSector(uint32_t PageAddress)
{
	return ((PageAddress*gW25qxxDev.PageSize)/gW25qxxDev.SectorSize);
}
 
uint32_t W25qxx_PageToBlock(uint32_t PageAddress)
{
	return ((PageAddress*gW25qxxDev.PageSize)/gW25qxxDev.BlockSize);
}
 
uint32_t W25qxx_SectorToBlock(uint32_t SectorAddress)
{
	return ((SectorAddress*gW25qxxDev.SectorSize)/gW25qxxDev.BlockSize);
}
 
uint32_t W25qxx_SectorToPage(uint32_t SectorAddress)
{
	return (SectorAddress*gW25qxxDev.SectorSize)/gW25qxxDev.PageSize;
}
 
uint32_t W25qxx_BlockToPage(uint32_t BlockAddress)
{
	return (BlockAddress*gW25qxxDev.BlockSize)/gW25qxxDev.PageSize;
}
 
bool W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;
	if(((NumByteToCheck_up_to_PageSize+OffsetInByte)>gW25qxxDev.PageSize)||(NumByteToCheck_up_to_PageSize==0))
		NumByteToCheck_up_to_PageSize=gW25qxxDev.PageSize-OffsetInByte;

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckPage:%d, Offset:%d, Bytes:%d begin...\r\n",Page_Address,OffsetInByte,NumByteToCheck_up_to_PageSize);
	#endif

	uint8_t	pBuffer[32];
	uint32_t	WorkAddress;
	uint32_t	i;
	for(i=OffsetInByte; i<gW25qxxDev.PageSize; i+=sizeof(pBuffer))
	{
        FLASH_SS_Clear(); 
		WorkAddress=(i+Page_Address*gW25qxxDev.PageSize);
		if (gW25qxxDev.ID >= W25Q256)
		{
			W25qxx_Spi(0x0C);
			W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25qxx_Spi(0x0B);
		}
		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);
		W25qxx_Spi(0);
		HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, sizeof(pBuffer), W25QXXH_SPI_TIMEOUT_MS);
		FLASH_SS_Set();
		for(uint8_t x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((gW25qxxDev.PageSize+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<gW25qxxDev.PageSize; i++)
		{
            FLASH_SS_Clear();
			WorkAddress=(i+Page_Address*gW25qxxDev.PageSize);
			W25qxx_Spi(0x0B);
			if(gW25qxxDev.ID>=W25Q256)
				W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
			W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
			W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
			W25qxx_Spi(WorkAddress & 0xFF);
			W25qxx_Spi(0);
			HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, 1, W25QXXH_SPI_TIMEOUT_MS);
            FLASH_SS_Set();
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckPage is Empty\n\r");
	W25qxx_Delay(100);
	#endif	

	gW25qxxDev.Lock=0;
	return true;

NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckPage is Not Empty\n\r");
	W25qxx_Delay(100);
	#endif
	gW25qxxDev.Lock=0;
	return false;
}
 
bool W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}

	gW25qxxDev.Lock=1;	
	if((NumByteToCheck_up_to_SectorSize>gW25qxxDev.SectorSize)||(NumByteToCheck_up_to_SectorSize==0))
	{
		NumByteToCheck_up_to_SectorSize=gW25qxxDev.SectorSize;
	}


	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckSector:%d, Offset:%d, Bytes:%d begin...\r\n",Sector_Address,OffsetInByte,NumByteToCheck_up_to_SectorSize);
	uint32_t StartTime = HAL_GetTick();
    W25qxx_Delay(100);
	#endif

	uint8_t	pBuffer[32];
	uint32_t	WorkAddress;
	uint32_t	i;

	for(i=OffsetInByte; i<gW25qxxDev.SectorSize; i+=sizeof(pBuffer))
	{
        FLASH_SS_Clear();
		WorkAddress=(i+Sector_Address*gW25qxxDev.SectorSize);
		W25qxx_Spi(0x0B);
		if (gW25qxxDev.ID >= W25Q256)
		{
			W25qxx_Spi(0x0C);
			W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25qxx_Spi(0x0B);
		}
		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);
		W25qxx_Spi(0);
		HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, sizeof(pBuffer), W25QXXH_SPI_TIMEOUT_MS);
		FLASH_SS_Set();
		for(uint8_t x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((gW25qxxDev.SectorSize+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<gW25qxxDev.SectorSize; i++)
		{
             FLASH_SS_Clear();
			WorkAddress=(i+Sector_Address*gW25qxxDev.SectorSize);
			W25qxx_Spi(0x0B);
			if(gW25qxxDev.ID>=W25Q256)
				W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
			W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
			W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
			W25qxx_Spi(WorkAddress & 0xFF);
			W25qxx_Spi(0);
			FLASH_SS_Set();
            HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, 1, W25QXXH_SPI_TIMEOUT_MS);

			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckSector is Empty\n\r");// in %d ms\r\n",HAL_GetTick()-StartTime);
	W25qxx_Delay(100);
	#endif	
	gW25qxxDev.Lock=0;
	return true;

NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckSector is Not Empty in %d ms\r\n",HAL_GetTick()-StartTime);
	W25qxx_Delay(100);
	#endif	
	gW25qxxDev.Lock=0;
	return false;
}
 
bool W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;	

	if((NumByteToCheck_up_to_BlockSize>gW25qxxDev.BlockSize)||(NumByteToCheck_up_to_BlockSize==0))
	{
		NumByteToCheck_up_to_BlockSize=gW25qxxDev.BlockSize;
	}

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev CheckBlock:%d, Offset:%d, Bytes:%d begin...\r\n",Block_Address,OffsetInByte,NumByteToCheck_up_to_BlockSize);
	W25qxx_Delay(100);
	#endif

	uint8_t	pBuffer[32];
	uint32_t	WorkAddress;
	uint32_t	i;

	for(i=OffsetInByte; i<gW25qxxDev.BlockSize; i+=sizeof(pBuffer))
	{
         FLASH_SS_Clear();
		WorkAddress=(i+Block_Address*gW25qxxDev.BlockSize);
		if (gW25qxxDev.ID >= W25Q256)
		{
			W25qxx_Spi(0x0C);
			W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		}
		else
		{
			W25qxx_Spi(0x0B);
		}

		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);
		W25qxx_Spi(0);
		HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, sizeof(pBuffer), W25QXXH_SPI_TIMEOUT_MS);
		FLASH_SS_Set();

		for(uint8_t x=0;x<sizeof(pBuffer);x++)
		{
			if(pBuffer[x]!=0xFF)
				goto NOT_EMPTY;		
		}			
	}	
	if((gW25qxxDev.BlockSize+OffsetInByte)%sizeof(pBuffer)!=0)
	{
		i-=sizeof(pBuffer);
		for( ; i<gW25qxxDev.BlockSize; i++)
		{
             FLASH_SS_Clear();
			WorkAddress=(i+Block_Address*gW25qxxDev.BlockSize);
			W25qxx_Spi(0x0B);
			if(gW25qxxDev.ID>=W25Q256)
				W25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
			W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
			W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
			W25qxx_Spi(WorkAddress & 0xFF);
			W25qxx_Spi(0);
			HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, 1, W25QXXH_SPI_TIMEOUT_MS);
			FLASH_SS_Set();
			if(pBuffer[0]!=0xFF)
				goto NOT_EMPTY;
		}
	}	
	#if (_W25QXX_DEBUG==1)
	W25qxx_Delay(100);
	#endif	
	gW25qxxDev.Lock=0;
	return true;

NOT_EMPTY:
	#if (_W25QXX_DEBUG==1)
	W25qxx_Delay(100);
	#endif	
	gW25qxxDev.Lock=0;
	return false;
}
 
eW25qxxStatus W25qxx_WriteByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev WriteByte 0x%02X at address %d begin...",pBuffer,WriteAddr_inBytes);
	uint32_t StartTime = HAL_GetTick();
	#endif

	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	FLASH_SS_Clear();

	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0x12);
		W25qxx_Spi((WriteAddr_inBytes & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0x02);
	}

	W25qxx_Spi((WriteAddr_inBytes & 0xFF0000) >> 16);
	W25qxx_Spi((WriteAddr_inBytes & 0xFF00) >> 8);
	W25qxx_Spi(WriteAddr_inBytes & 0xFF);
	W25qxx_Spi(pBuffer);
	FLASH_SS_Set();

	W25qxx_WaitForWriteEnd();

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev WriteByte done after %d ms\r\n",HAL_GetTick()-StartTime);
	#endif

	gW25qxxDev.Lock=0;

	return 0;
}
 
eW25qxxStatus W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;

	if(((NumByteToWrite_up_to_PageSize+OffsetInByte)>gW25qxxDev.PageSize)||(NumByteToWrite_up_to_PageSize==0))
	{
		NumByteToWrite_up_to_PageSize=gW25qxxDev.PageSize-OffsetInByte;
	}

	if((OffsetInByte+NumByteToWrite_up_to_PageSize) > gW25qxxDev.PageSize)
	{
		NumByteToWrite_up_to_PageSize = gW25qxxDev.PageSize-OffsetInByte;
	}


	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToWrite_up_to_PageSize);
	W25qxx_Delay(100);
	#endif	

	W25qxx_WaitForWriteEnd();
    W25qxx_WriteEnable();
    FLASH_SS_Clear();
	Page_Address = (Page_Address*gW25qxxDev.PageSize)+OffsetInByte;	
	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0x12);
		W25qxx_Spi((Page_Address & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0x02);
	}

    W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
    W25qxx_Spi((Page_Address & 0xFF00) >> 8);
    W25qxx_Spi(Page_Address&0xFF);
  
	HAL_SPI_Transmit(W25QXXH_SPI_HANDLE, pBuffer, NumByteToWrite_up_to_PageSize, W25QXXH_SPI_TIMEOUT_MS);
    FLASH_SS_Set();
    W25qxx_WaitForWriteEnd();

	#if (_W25QXX_DEBUG==1)
    uint32_t StartTime = HAL_GetTick();
	for(uint32_t i=0;i<NumByteToWrite_up_to_PageSize ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n");
			W25qxx_Delay(10);			
		}
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "0x%02X,",pBuffer[i]);
	}	
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n");
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev WritePage done after %d ms\n\r", StartTime );
	W25qxx_Delay(100);
	#endif	

	W25qxx_Delay(1);
	gW25qxxDev.Lock=0;
    
    return 0;
}
 
eW25qxxStatus W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
	if((NumByteToWrite_up_to_SectorSize>gW25qxxDev.SectorSize)||(NumByteToWrite_up_to_SectorSize==0))
	{
		NumByteToWrite_up_to_SectorSize=gW25qxxDev.SectorSize;
	}

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "+++gW25qxxDev WriteSector:%d, Offset:%d ,Write %d Bytes, begin...\r\n",Sector_Address,OffsetInByte,NumByteToWrite_up_to_SectorSize);
	W25qxx_Delay(100);
	#endif

	if(OffsetInByte>=gW25qxxDev.SectorSize)
	{
		#if (_W25QXX_DEBUG==1)
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev WriteSector Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return -1;
	}	
	uint32_t	StartPage;
	int32_t		BytesToWrite;
	uint32_t	LocalOffset;	

	if((OffsetInByte+NumByteToWrite_up_to_SectorSize) > gW25qxxDev.SectorSize)
	{
		BytesToWrite = gW25qxxDev.SectorSize-OffsetInByte;
	}
	else
	{
		BytesToWrite = NumByteToWrite_up_to_SectorSize;	
	}

	StartPage = W25qxx_SectorToPage(Sector_Address)+(OffsetInByte/gW25qxxDev.PageSize);
	LocalOffset = OffsetInByte%gW25qxxDev.PageSize;	

	do
	{		
		W25qxx_WritePage(pBuffer,StartPage,LocalOffset,BytesToWrite);
		StartPage++;
		BytesToWrite-=gW25qxxDev.PageSize-LocalOffset;
		pBuffer += gW25qxxDev.PageSize - LocalOffset;
		LocalOffset=0;
	}while(BytesToWrite>0);		

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev WriteSector Done\r\n");
	W25qxx_Delay(100);
	#endif	

    return 0;
}
 
eW25qxxStatus W25qxx_WriteBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
	if((NumByteToWrite_up_to_BlockSize>gW25qxxDev.BlockSize)||(NumByteToWrite_up_to_BlockSize==0))
	{
		NumByteToWrite_up_to_BlockSize=gW25qxxDev.BlockSize;
	}

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "+++gW25qxxDev WriteBlock:%d, Offset:%d ,Write %d Bytes, begin...\r\n",Block_Address,OffsetInByte,NumByteToWrite_up_to_BlockSize);
	W25qxx_Delay(100);
	#endif	

	if(OffsetInByte>=gW25qxxDev.BlockSize)
	{
		#if (_W25QXX_DEBUG==1)
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev WriteBlock Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return -1;
	}	

	uint32_t	StartPage;
	int32_t		BytesToWrite;
	uint32_t	LocalOffset;	
	if((OffsetInByte+NumByteToWrite_up_to_BlockSize) > gW25qxxDev.BlockSize)
	{
		BytesToWrite = gW25qxxDev.BlockSize-OffsetInByte;
	}
	else
	{
		BytesToWrite = NumByteToWrite_up_to_BlockSize;	
	}

	StartPage = W25qxx_BlockToPage(Block_Address)+(OffsetInByte/gW25qxxDev.PageSize);
	LocalOffset = OffsetInByte%gW25qxxDev.PageSize;

	do
	{		
		W25qxx_WritePage(pBuffer,StartPage,LocalOffset,BytesToWrite);
		StartPage++;
		BytesToWrite-=gW25qxxDev.PageSize-LocalOffset;
		pBuffer += gW25qxxDev.PageSize - LocalOffset;
		LocalOffset=0;
	}while(BytesToWrite>0);

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev WriteBlock Done\r\n");
	W25qxx_Delay(100);
	#endif	

    return 0;
}
 
eW25qxxStatus W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}

	gW25qxxDev.Lock=1;

	#if (_W25QXX_DEBUG==1)
	uint32_t StartTime = HAL_GetTick();
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ReadByte at address %d begin...\r\n",Bytes_Address);
	#endif

    FLASH_SS_Clear();
	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0x0C);
		W25qxx_Spi((Bytes_Address & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0x0B);
	}

	W25qxx_Spi((Bytes_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Bytes_Address& 0xFF00) >> 8);
	W25qxx_Spi(Bytes_Address & 0xFF);
	W25qxx_Spi(0);
	*pBuffer = W25qxx_Spi(W25QXX_DUMMY_BYTE);
    FLASH_SS_Set();

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ReadByte 0x%02X done after %d ms\r\n", HAL_GetTick()-StartTime);
	#endif

	gW25qxxDev.Lock=0;
    
    return 0;
}
 
eW25qxxStatus W25qxx_ReadBytes(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}

	gW25qxxDev.Lock=1;

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ReadBytes at Address:%d, %d Bytes  begin...\r\n",ReadAddr,NumByteToRead);
    uint32_t StartTime = HAL_GetTick();
	#endif	

    FLASH_SS_Clear();
	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0x0C);
		W25qxx_Spi((ReadAddr & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0x0B);
	}

    W25qxx_Spi((ReadAddr & 0xFF0000) >> 16);
    W25qxx_Spi((ReadAddr& 0xFF00) >> 8);
    W25qxx_Spi(ReadAddr & 0xFF);
	W25qxx_Spi(0);
	HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, NumByteToRead, 2000);
    FLASH_SS_Set();

	#if (_W25QXX_DEBUG==1)
	for(uint32_t i=0;i<NumByteToRead ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n");
			W25qxx_Delay(10);
		}
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "0x%02X,",pBuffer[i]);
	}
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\nw25qxx ReadBytes done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif	

	W25qxx_Delay(1);
	gW25qxxDev.Lock=0;
    
    return 0;
}
 
eW25qxxStatus W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
	while(gW25qxxDev.Lock==1)
	{
		W25qxx_Delay(1);
	}
	gW25qxxDev.Lock=1;
	if((NumByteToRead_up_to_PageSize>gW25qxxDev.PageSize)||(NumByteToRead_up_to_PageSize==0))
	{
		NumByteToRead_up_to_PageSize=gW25qxxDev.PageSize;
	}

	if((OffsetInByte+NumByteToRead_up_to_PageSize) > gW25qxxDev.PageSize)
	{
		NumByteToRead_up_to_PageSize = gW25qxxDev.PageSize-OffsetInByte;
	}

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ReadPage:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Page_Address,OffsetInByte,NumByteToRead_up_to_PageSize);
	W25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
	#endif	

	Page_Address = Page_Address*gW25qxxDev.PageSize+OffsetInByte;
    FLASH_SS_Clear();
	if (gW25qxxDev.ID >= W25Q256)
	{
		W25qxx_Spi(0x0C);
		W25qxx_Spi((Page_Address & 0xFF000000) >> 24);
	}
	else
	{
		W25qxx_Spi(0x0B);
	}

	W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Page_Address& 0xFF00) >> 8);
	W25qxx_Spi(Page_Address & 0xFF);
	W25qxx_Spi(0);
	HAL_SPI_Receive(W25QXXH_SPI_HANDLE, pBuffer, NumByteToRead_up_to_PageSize, W25QXXH_SPI_TIMEOUT_MS);
    FLASH_SS_Set(); 

	#if (_W25QXX_DEBUG==1)
	for(uint32_t i=0;i<NumByteToRead_up_to_PageSize ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n");
			W25qxx_Delay(10);
		}
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "0x%02X,",pBuffer[i]);
	}	
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n");
    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ReadPage done after %d \n\r", StartTime);
	W25qxx_Delay(100);
	#endif

	W25qxx_Delay(1);
	gW25qxxDev.Lock=0;
    
    return 0;
}
 
eW25qxxStatus W25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize)
{	
	if((NumByteToRead_up_to_SectorSize>gW25qxxDev.SectorSize)||(NumByteToRead_up_to_SectorSize==0))
	{
		NumByteToRead_up_to_SectorSize=gW25qxxDev.SectorSize;
	}

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "+++gW25qxxDev ReadSector:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Sector_Address,OffsetInByte,NumByteToRead_up_to_SectorSize);
	W25qxx_Delay(100);
	#endif	

	if(OffsetInByte>=gW25qxxDev.SectorSize)
	{
		#if (_W25QXX_DEBUG==1)
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev ReadSector Faild!\r\n");
		W25qxx_Delay(100);
		#endif	
		return -1;
	}	

	uint32_t	StartPage;
	int32_t		BytesToRead;
	uint32_t	LocalOffset;	
	if((OffsetInByte+NumByteToRead_up_to_SectorSize) > gW25qxxDev.SectorSize)
	{
		BytesToRead = gW25qxxDev.SectorSize-OffsetInByte;
	}

	else
	{
		BytesToRead = NumByteToRead_up_to_SectorSize;	
	}

	StartPage = W25qxx_SectorToPage(Sector_Address)+(OffsetInByte/gW25qxxDev.PageSize);
	LocalOffset = OffsetInByte%gW25qxxDev.PageSize;	

	do
	{		
		W25qxx_ReadPage(pBuffer,StartPage,LocalOffset,BytesToRead);
		StartPage++;
		BytesToRead-=gW25qxxDev.PageSize-LocalOffset;
		pBuffer += gW25qxxDev.PageSize - LocalOffset;
		LocalOffset=0;
	}while(BytesToRead>0);		

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev ReadSector Done\r\n");
	W25qxx_Delay(100);
	#endif	

    return 0;
}
 
eW25qxxStatus W25qxx_ReadBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t	NumByteToRead_up_to_BlockSize)
{
	if((NumByteToRead_up_to_BlockSize>gW25qxxDev.BlockSize)||(NumByteToRead_up_to_BlockSize==0))
	{
		NumByteToRead_up_to_BlockSize=gW25qxxDev.BlockSize;
	}

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "+++gW25qxxDev ReadBlock:%d, Offset:%d ,Read %d Bytes, begin...\r\n",Block_Address,OffsetInByte,NumByteToRead_up_to_BlockSize);
	W25qxx_Delay(100);
	#endif	

	if(OffsetInByte>=gW25qxxDev.BlockSize)
	{
		#if (_W25QXX_DEBUG==1)
		DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "gW25qxxDev ReadBlock Failed!\r\n");
		W25qxx_Delay(100);
		#endif	
		return -1;
	}

	uint32_t	StartPage;
	int32_t		BytesToRead;
	uint32_t	LocalOffset;	

	if((OffsetInByte+NumByteToRead_up_to_BlockSize) > gW25qxxDev.BlockSize)
	{
		BytesToRead = gW25qxxDev.BlockSize-OffsetInByte;
	}

	else
	{
		BytesToRead = NumByteToRead_up_to_BlockSize;
	}

	StartPage = W25qxx_BlockToPage(Block_Address)+(OffsetInByte/gW25qxxDev.PageSize);
	LocalOffset = OffsetInByte%gW25qxxDev.PageSize;	

	do
	{		
		W25qxx_ReadPage(pBuffer,StartPage,LocalOffset,BytesToRead);
		StartPage++;
		BytesToRead-=gW25qxxDev.PageSize-LocalOffset;
		pBuffer += gW25qxxDev.PageSize - LocalOffset;
		LocalOffset=0;
	}while(BytesToRead>0);

	#if (_W25QXX_DEBUG==1)
	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "---gW25qxxDev ReadBlock Done\r\n");
	W25qxx_Delay(100);
	#endif	

    return 0;
}
 
#ifdef ENABLE_TESTS_DEFINITIONS

bool W25qxx_Test()
{
    uint8_t Testbuf[32] = {0};
    uint8_t WriteBuf[32] = "Hello gW25qxxDev\r\n" ;

    eW25qxxStatus status = W25qxx_Init();
    
    status |= W25qxx_ReadSector((uint8_t *)Testbuf,0,0,sizeof(Testbuf));
    
    status |= W25qxx_WriteSector((uint8_t *)WriteBuf,0,0,sizeof(WriteBuf));
    
    status |= W25qxx_ReadSector((uint8_t *)Testbuf,0,0,sizeof(Testbuf));
    
    return (0 == status);
    
}

#endif
