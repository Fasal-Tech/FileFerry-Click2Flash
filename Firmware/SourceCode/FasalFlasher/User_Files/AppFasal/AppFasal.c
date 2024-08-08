/**
 * @file AppFasal.c
 * @author Vishal Keshava Murthy
 * @brief Fasal Application Implementation
 * @version 0.1
 * @date 2023-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>

#include "AppFasal.h"
#include "AppCommon.h"
#include "AppIndication.h"
#include "Console.h"
#include "SoftTimer.h"
#include "DebugPrint.h"
#include "AppStorage.h"
#include "PushButton.h"
#include "AppStorageDataStructures.h"
#include "AppFlash_API.h"
#include "AppSD_API.h"
#include "ConfigSetting.h"
#include "xmodem.h"
#include "AppProfiler.h"
#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Map App state with indication state
 *
 */
static const eAppIndicationStates_t gcIndicationToAppStateMap[eFASAL_MAX_STATE] =
{
		[eFASAL_APP_INIT] 				= eIND_BLUE_0,
		[eFASAL_APP_STARTUP_MSG] 		= eIND_BLUE_250MS,
		[eFASAL_APP_BUTTON_WAIT] 		= eIND_BLUE_500MS,
		[eFASAL_APP_SD_INIT] 			= eIND_BLUE_1000MS,
		[eFASAL_APP_SD_CHECK] 			= eIND_BLUE_1000MS,
		[eFASAL_APP_FLASH_INIT] 		= eIND_BLUE_1000MS,
		[eFASAL_APP_MODE_SELECTION]		= eIND_BLUE_1000MS,
		[eFASAL_APP_SD_FLASH_TRANSFER] 	= eIND_YELLOW_1000MS,
		[eFASAL_APP_XMODEM_TRANSFER] 	= eIND_YELLOW_1000MS,
		[eFASAL_APP_CRC_COMPARE] 		= eIND_YELLOW_1000MS,
		[eFASAL_APP_TRANSFER_SUCCESS] 	= eIND_GREEN_0,
		[eFASAL_APP_SD_FAIL] 			= eIND_RED_250MS,
		[eFASAL_APP_SD_FILE_FAIL] 		= eIND_RED_250MS,
		[eFASAL_APP_FLASH_FAIL] 		= eIND_RED_250MS,
		[eFASAL_APP_TRANSFER_FAIL] 		= eIND_RED_250MS,
		[eFASAL_APP_CRC_FAIL] 			= eIND_RED_250MS,
		[eFASAL_APP_END] 				= eIND_NO_CHANGE,
};


/**
 * @brief Initialize all Application modules here
 *
 */
static void AppFasal_Init()
{
	SoftTimer_Init();
	AppIndicate_Init();

	AppIndicate_SetState(eIND_BLUE_250MS);

	Console_Init();
}


/**
 * @brief Application Run
 *
 * @return eAppFasalStates_t
 *
 */
eAppFasalStates_t AppFasal_Run()
{
	static eAppFasalStates_t NextState = eFASAL_APP_INIT ;

	AppIndicate_SetState(gcIndicationToAppStateMap[NextState]);

	DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n>> App State %u", NextState );

	switch(NextState)
	{
		case eFASAL_APP_INIT:
			AppFasal_Init();
			NextState = eFASAL_APP_STARTUP_MSG;
			break;

		case eFASAL_APP_STARTUP_MSG:
			AppCommon_StartUpMessagePrint();
			AppCommon_PrintLineBreak();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Press Flash Button for Initiating Transfer");
			NextState = eFASAL_APP_BUTTON_WAIT;
			break;

		case eFASAL_APP_BUTTON_WAIT:
		{
			bool IsButtonPressed = PushButton_IsFlashButtonPressed();
			NextState = (true == IsButtonPressed)? eFASAL_APP_FLASH_INIT: eFASAL_APP_BUTTON_WAIT ;
			break;
		}

		case eFASAL_APP_FLASH_INIT:
		{
			AppStorage_SetPower(true);	/**< Set power to External Flash prior to Initializing the same*/
			eStorageFSStatus_t FlashInitStatus = FlashFs_API_Init();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Flash Init %s", AppCommon_GetStatusString(FlashInitStatus));

			NextState = (eFS_SUCCESS == FlashInitStatus)? eFASAL_APP_MODE_SELECTION: eFASAL_APP_FLASH_FAIL ;
			break;
		}

		case eFASAL_APP_MODE_SELECTION:
		{
			eTransferMode_t TransferMode = AppStorage_GetCurrentTransferMode();
			switch(TransferMode)
			{
				case eTX_MODE_SDCARD_TO_FLASH:
					Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transfer Mode: SD card to flash selected");
					NextState = eFASAL_APP_SD_INIT;
					break;

				case eTX_MODE_XMODEM_TO_FLASH:
					Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transfer Mode: X-Modem to flash selected");
					NextState = eFASAL_APP_XMODEM_TRANSFER;
					break;

				case eTX_MODE_MAX:
				default:
					NextState = eFASAL_APP_SD_INIT;
					break;
			}
			break;
		}

		case eFASAL_APP_SD_INIT:
		{
			eStorageFSStatus_t SDStatus = SDFs_API_Init();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> SD-Card Init %s", AppCommon_GetStatusString(SDStatus));
			NextState = (eFS_SUCCESS == SDStatus)? eFASAL_APP_SD_CHECK: eFASAL_APP_SD_FAIL;
			break;
		}

		case eFASAL_APP_SD_CHECK:
		{
			eStorageFSStatus_t SDFileStatus = SDFs_API_GetGoldenFileStatus();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Golden Image in SD-Card status %s", AppCommon_GetStatusString(SDFileStatus));
			NextState = (eFS_SUCCESS == SDFileStatus)? eFASAL_APP_SD_FLASH_TRANSFER: eFASAL_APP_SD_FILE_FAIL ;
			break;
		}

		case eFASAL_APP_SD_FLASH_TRANSFER:
		{
			FlashFs_API_DeleteGoldenImageFile();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transferring Golden Image file from SD-Card to Flash. Estimated Time to Completion: 30s");
			eStorageFSStatus_t TransferStatus = AppStorage_TransferGoldenImageFileFromSDToFlash();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> File Transfer from SD-Card to Flash %s", AppCommon_GetStatusString(TransferStatus));

	#ifdef FORCE_DISABLE_FILE_CRC_CHECK
			NextState = (eFS_SUCCESS == TransferStatus)? eFASAL_APP_TRANSFER_SUCCESS: eFASAL_APP_TRANSFER_FAIL;
	#else
			NextState = (eFS_SUCCESS == TransferStatus)? eFASAL_APP_CRC_COMPARE: eFASAL_APP_TRANSFER_FAIL;
	#endif
			break;
		}


		case eFASAL_APP_XMODEM_TRANSFER:
		{
			FlashFs_API_DeleteGoldenImageFile();
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Send Golden Image over X-modem for Update. Estimated Time to Completion: 45s \r\n");
			xmodem_status xModemTransferstatus = xmodem_API_receive();

			NextState = (X_COMPLETE == xModemTransferstatus)? eFASAL_APP_TRANSFER_SUCCESS: eFASAL_APP_TRANSFER_FAIL;
			break;
		}

		case eFASAL_APP_CRC_COMPARE:
		{
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Computing CRC of files in SD card and Flash storage... Estimated Time to Completion: 5s");
			bool IsCRCMatching = false;

			eStorageFSStatus_t CRCComputeStatus = AppStorage_CompareCRCOfGoldenImageFileInSDAndFlash(&IsCRCMatching);
			if(eFS_SUCCESS == CRCComputeStatus)
			{
				Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transferred Files integrity verified ");
				NextState = (true == IsCRCMatching)?eFASAL_APP_TRANSFER_SUCCESS : eFASAL_APP_CRC_FAIL;
			}
			else
			{
				NextState = eFASAL_APP_CRC_FAIL;
			}

			break;
		}

		case eFASAL_APP_CRC_FAIL:
		{
			AppCommon_AccumlateErrorCode(eERR_CRC_FAILURE);
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> SD-Card and flash file CRC mismatch ");
			NextState = eFASAL_APP_END;

		}

		case eFASAL_APP_TRANSFER_SUCCESS:
		{
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> File transfer successfully complete! ");
			NextState = eFASAL_APP_END;
			break;
		}

		case eFASAL_APP_SD_FAIL:
		{
			AppCommon_AccumlateErrorCode(eERR_SDCARD_NOT_FOUND);
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> SD-Card and file system setup failure ");
			NextState = eFASAL_APP_END;
			break;
		}

		case eFASAL_APP_SD_FILE_FAIL:
		{
			AppCommon_AccumlateErrorCode(eERR_SDCARD_FILE_NOT_FOUND);
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Golden Image not found in in SD-Card!");
			NextState = eFASAL_APP_END;
			break;
		}

		case eFASAL_APP_FLASH_FAIL:
		{
			AppCommon_AccumlateErrorCode(eERR_FLASH_NOT_FOUND);
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Flash failure!");
			NextState = eFASAL_APP_END;
			break;
		}

		case eFASAL_APP_TRANSFER_FAIL:
		{
			AppCommon_AccumlateErrorCode(eERR_FLASH_TRANSFER_FAILURE);
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> File transfer Fail!");

			FlashFs_API_DeleteGoldenImageFile();

			NextState = eFASAL_APP_END;
			break;
		}

		case eFASAL_APP_END:
		{
			Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Application Error Code: %04X", AppCommon_GetErrorCode());

			AppStorage_SetPower(false); /**< Stop powering the external flash since transfer operation is complete*/
			AppCommon_ResetErrorCode();	/**< Errors from previous run if any must be cleared here*/

			static const uint32_t cRESULT_DISPLAY_TIME_MS = 5000u;
			SoftTimer_DelayMS(cRESULT_DISPLAY_TIME_MS);
			NextState = eFASAL_APP_STARTUP_MSG;
			break;
		}

		case eFASAL_MAX_STATE:
			break;

		default:
			break;
		}

	return NextState;
}

///////////////////////////////////////////////////////////////////////////////
