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
#include "AppConfiguration.h"
#include "AppFasal.h"
#include "AppFlash_API.h"
#include "AppIndication.h"
#include "AppProfiler.h"
#include "AppResetAndError.h"
#include "AppSD_API.h"
#include "AppStorage.h"
#include "AppStorageDataStructures.h"
#include "ConfigSetting.h"
#include "Console.h"
#include "DebugPrint.h"
#include "PushButton.h"
#include "SoftTimer.h"
#include "xmodem.h"
#include "Version.h"

///////////////////////////////////////////////////////////////////////////////

// clang-format off

/**
 * @brief Map App state with indication state
 *
 */
static const eAppIndicationStates_t gcIndicationToAppStateMap[eAPP_MAX_STATE] =
{
		[eAPP_INIT] 			= eIND_BLUE_0,
		[eAPP_STARTUP_MSG] 		= eIND_BLUE_250MS,
		[eAPP_BUTTON_WAIT] 		= eIND_BLUE_500MS,
		[eAPP_SD_INIT] 			= eIND_BLUE_1000MS,
		[eAPP_SD_CHECK] 		= eIND_BLUE_1000MS,
		[eAPP_FLASH_INIT] 		= eIND_BLUE_1000MS,
		[eAPP_MODE_SELECTION]	= eIND_BLUE_1000MS,
		[eAPP_SD_FLASH_TRANSFER]= eIND_YELLOW_1000MS,
		[eAPP_XMODEM_TRANSFER] 	= eIND_YELLOW_1000MS,
		[eAPP_CRC_COMPARE] 		= eIND_YELLOW_1000MS,
		[eAPP_TRANSFER_SUCCESS] = eIND_GREEN_0,
		[eAPP_SD_FAIL] 			= eIND_RED_250MS,
		[eAPP_SD_FILE_FAIL] 	= eIND_RED_250MS,
		[eAPP_FLASH_FAIL] 		= eIND_RED_250MS,
		[eAPP_TRANSFER_FAIL] 	= eIND_RED_250MS,
		[eAPP_CRC_FAIL] 		= eIND_RED_250MS,
		[eAPP_END] 				= eIND_NO_CHANGE,
};

// clang-format on

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
 * @brief Start up message print
 *
 */
static void AppFasal_StartUpMessagePrint()
{
    Console_PrintDelimiter();
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Fasal. Grow More, Grow Better!\r\n Wolkus Technology Solutions Private Limited, Bangalore, India.");
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Device: HW version: %d.%d, FW version: %d.%d", HARDWARE_VERSION_MAJOR, HARDWARE_VERSION_MINOR, FW_VERSION_MAJOR,
                  FW_VERSION_MINOR);
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Binaries compiled on %s at %s", COMPILE_DATE, COMPILE_TIME);
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Build for Fasal Flasher Board");
    Console_PrintDelimiter();
}

/**
 * @brief Application Run
 *
 * @return eAppFasalStates_t
 *
 */
eAppFasalStates_t AppFasal_Run()
{
    static eAppFasalStates_t gsNextState = eAPP_INIT;

    AppIndicate_SetState(gcIndicationToAppStateMap[gsNextState]);

    DEBUG_PRINT(eCONSOLE_PRINT_LVL0, "\r\n>> App State %u", gsNextState);

    switch (gsNextState)
    {
        case eAPP_INIT:
            AppFasal_Init();
            gsNextState = eAPP_STARTUP_MSG;
            break;

        case eAPP_STARTUP_MSG:
            AppFasal_StartUpMessagePrint();
            Console_PrintLineBreak();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Press Flash Button for Initiating Transfer");
            gsNextState = eAPP_BUTTON_WAIT;
            break;

        case eAPP_BUTTON_WAIT:
        {
            bool IsButtonPressed = PushButton_IsFlashButtonPressed();
            gsNextState = (true == IsButtonPressed) ? eAPP_FLASH_INIT : eAPP_BUTTON_WAIT;
            break;
        }

        case eAPP_FLASH_INIT:
        {
            AppStorage_SetPower(true); ///< Set power to External Flash prior to Initializing the same
            eStorageFSStatus_t FlashInitStatus = FlashFs_API_Init();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> DUT Flash Setup %s", AppCommon_GetStatusString(FlashInitStatus));

            gsNextState = (eFS_SUCCESS == FlashInitStatus) ? eAPP_MODE_SELECTION : eAPP_FLASH_FAIL;
            break;
        }

        case eAPP_MODE_SELECTION:
        {
            eTransferMode_t TransferMode = AppStorage_GetCurrentTransferMode();
            switch (TransferMode)
            {
                case eTX_MODE_SDCARD_TO_FLASH:
                    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transfer Mode: SD card to flash selected");
                    gsNextState = eAPP_SD_INIT;
                    break;

                case eTX_MODE_XMODEM_TO_FLASH:
                    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transfer Mode: X-Modem to flash selected");
                    gsNextState = eAPP_XMODEM_TRANSFER;
                    break;

                case eTX_MODE_MAX:
                default:
                    gsNextState = eAPP_SD_INIT;
                    break;
            }
            break;
        }

        case eAPP_SD_INIT:
        {
            eStorageFSStatus_t SDStatus = SDFs_API_Init();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> SD-Card Init %s", AppCommon_GetStatusString(SDStatus));
            gsNextState = (eFS_SUCCESS == SDStatus) ? eAPP_SD_CHECK : eAPP_SD_FAIL;
            break;
        }

        case eAPP_SD_CHECK:
        {
            eStorageFSStatus_t SDFileStatus = SDFs_API_GetGoldenFileStatus();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Golden Image in SD-Card status %s", AppCommon_GetStatusString(SDFileStatus));
            gsNextState = (eFS_SUCCESS == SDFileStatus) ? eAPP_SD_FLASH_TRANSFER : eAPP_SD_FILE_FAIL;
            break;
        }

        case eAPP_SD_FLASH_TRANSFER:
        {
            FlashFs_API_DeleteGoldenImageFile();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transferring Golden Image file from SD-Card to Flash. Estimated Time to Completion: 30s");
            eStorageFSStatus_t TransferStatus = AppStorage_TransferGoldenImageFileFromSDToFlash();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> File Transfer from SD-Card to Flash %s", AppCommon_GetStatusString(TransferStatus));

#ifdef FORCE_DISABLE_FILE_CRC_CHECK
            gsNextState = (eFS_SUCCESS == TransferStatus) ? eAPP_TRANSFER_SUCCESS : eAPP_TRANSFER_FAIL;
#else
            gsNextState = (eFS_SUCCESS == TransferStatus) ? eAPP_CRC_COMPARE : eAPP_TRANSFER_FAIL;
#endif
            break;
        }

        case eAPP_XMODEM_TRANSFER:
        {
            FlashFs_API_DeleteGoldenImageFile();
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Send Golden Image over X-modem for Update. Estimated Time to Completion: 45s \r\n");
            xmodem_status xModemTransferstatus = xmodem_API_receive();

            gsNextState = (X_COMPLETE == xModemTransferstatus) ? eAPP_TRANSFER_SUCCESS : eAPP_TRANSFER_FAIL;
            break;
        }

        case eAPP_CRC_COMPARE:
        {
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Computing CRC of files in SD card and Flash storage... Estimated Time to Completion: 5s");
            bool IsCRCMatching = false;

            eStorageFSStatus_t CRCComputeStatus = AppStorage_CompareCRCOfGoldenImageFileInSDAndFlash(&IsCRCMatching);
            if (eFS_SUCCESS == CRCComputeStatus)
            {
                Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Transferred Files integrity verified ");
                gsNextState = (true == IsCRCMatching) ? eAPP_TRANSFER_SUCCESS : eAPP_CRC_FAIL;
            }
            else
            {
                gsNextState = eAPP_CRC_FAIL;
            }

            break;
        }

        case eAPP_CRC_FAIL:
        {
            AppCommon_AccumlateErrorCode(eERR_CRC_FAILURE);
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> SD-Card and flash file CRC mismatch ");
            gsNextState = eAPP_END;
        }

        case eAPP_TRANSFER_SUCCESS:
        {
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> File transfer successfully complete! ");
            gsNextState = eAPP_END;
            break;
        }

        case eAPP_SD_FAIL:
        {
            AppCommon_AccumlateErrorCode(eERR_SDCARD_NOT_FOUND);
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> SD-Card and file system setup failure ");
            gsNextState = eAPP_END;
            break;
        }

        case eAPP_SD_FILE_FAIL:
        {
            AppCommon_AccumlateErrorCode(eERR_SDCARD_FILE_NOT_FOUND);
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Golden Image not found in in SD-Card!");
            gsNextState = eAPP_END;
            break;
        }

        case eAPP_FLASH_FAIL:
        {
            AppCommon_AccumlateErrorCode(eERR_FLASH_NOT_FOUND);
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Ensure DUT Flash is connected, and DUT MCU is empty/held in reset");
            gsNextState = eAPP_END;
            break;
        }

        case eAPP_TRANSFER_FAIL:
        {
            AppCommon_AccumlateErrorCode(eERR_FLASH_TRANSFER_FAILURE);
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> File transfer Fail!");

            FlashFs_API_DeleteGoldenImageFile();

            gsNextState = eAPP_END;
            break;
        }

        case eAPP_END:
        {
            Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Application Error Code: %04X", AppCommon_GetErrorCode());

            AppStorage_SetPower(false); ///< Stop powering the external flash since transfer operation is complete
            AppCommon_ResetErrorCode(); ///< Errors from previous run if any must be cleared here

            static const uint32_t cRESULT_DISPLAY_TIME_MS = 5000u;
            SoftTimer_DelayMS(cRESULT_DISPLAY_TIME_MS);
            gsNextState = eAPP_STARTUP_MSG;
            break;
        }

        case eAPP_MAX_STATE:
            break;

        default:
            break;
    }

    return gsNextState;
}

///////////////////////////////////////////////////////////////////////////////
