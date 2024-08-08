/**
 * @file AppCommon.h
 * @author Vishal Keshava Murthy
 * @brief Interface of Common functions used in the application
 * @version 0.1
 * @date 2023-06-03
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPCOMMON_APPCOMMON_H_
#define APPCOMMON_APPCOMMON_H_

///////////////////////////////////////////////////////////////////////////////

#define LOOP_FOREVER {while(1);} /**< Utility MACRO to loop forever */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Enum for status helper
 *
 */
typedef enum
{
	eSUCCESS_STRING,
	eFAILURE_STRING,
	eSTATUS_STRING_MAX
}eStatusPrintHelperEnum_t;

/**
 * @brief  Possible STM32 system reset causes
 *
 */
typedef enum
{
    eRESET_CAUSE_UNKNOWN = 0,
	eRESET_CAUSE_WAKEUP,
    eRESET_CAUSE_LOW_POWER_RESET,
    eRESET_CAUSE_WINDOW_WATCHDOG_RESET,
    eRESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    eRESET_CAUSE_SOFTWARE_RESET,
    eRESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    eRESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
    eRESET_CAUSE_BROWNOUT_RESET,
	eRESET_CAUSE_MAX
} eDeviceResetCause_t;


/**
 * @brief  Possible Errors in application
 *
 */
typedef enum
{
	eERR_NO_ERRORS 					= 0x0000,

	eERR_SDCARD_NOT_FOUND 			= 0x0001,
	eERR_SDCARD_FILE_NOT_FOUND 		= 0x0002,

	eERR_FLASH_NOT_FOUND	 		= 0x0004,
	eERR_FLASH_TRANSFER_FAILURE	 	= 0x0008,

	eERR_CRC_FAILURE 				= 0x0010,

	eERR_UNUSED_1			 		= 0x0020,
	eERR_UNUSED_2		 			= 0x0040,
	eERR_UNUSED_3		 			= 0x0080,
	eERR_UNUSED_4				 	= 0x0100,
	eERR_UNUSED_5					= 0X0200,
	eERR_UNUSED_6				 	= 0x0400,
	eERR_UNUSED_7	 				= 0x0800,

	eERR_STM_HAL_FAILURE 			= 0x1000,
	eERR_ARM_FAULT 					= 0x2000,
	eERR_ASSERTION_FAILURE 			= 0x4000,
	eERR_SLEEP_FAILURE 				= 0x8000,

}eDeviceErrorCode_t;


///////////////////////////////////////////////////////////////////////////////


void AppCommon_PrintDelimiter();
void AppCommon_PrintLineBreak();
void AppCommon_StartUpMessagePrint();
void AppCommon_HALErrorHandler();
void AppCommon_STMFaultHandler();
const char* const AppCommon_GetStatusString(int status);
void __assert_func(const char *file, int line, const char *func, const char *failedexpr);

void AppCommon_DetermineResetCause();
eDeviceResetCause_t AppCommon_GetCachedResetCause();

eDeviceErrorCode_t AppCommon_GetErrorCode();
void AppCommon_AccumlateErrorCode(eDeviceErrorCode_t err);
void AppCommon_ResetErrorCode();

///////////////////////////////////////////////////////////////////////////////

#endif /* APPCOMMON_APPCOMMON_H_ */
