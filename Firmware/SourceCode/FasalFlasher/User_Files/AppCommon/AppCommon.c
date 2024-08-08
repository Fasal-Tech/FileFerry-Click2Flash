/**
 * @file AppCommon.c
 * @author Vishal Keshava Murthy
 * @brief Definition of Common functions used in the application. Fault handlers are also defined here
 * @version 0.1
 * @date 2023-06-03
 *
 * @copyright Copyright (c) 2023
 *
 *
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "AppCommon.h"
#include "Version.h"
#include "AppIndication.h"
#include "Console.h"
#include "AppCommon.h"
#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

static eDeviceResetCause_t gDeviceResetCause = eRESET_CAUSE_UNKNOWN;	/**< Global that maintains reset cause*/
static eDeviceErrorCode_t gErrorCode = eERR_NO_ERRORS;					/**< Global that maintains error cause*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Map @ref eStatusPrintHelperEnum_t to string. Used by @ref AppCommon_GetStatusString
 *
 */
static const char* gcStatusStringHelper[eSTATUS_STRING_MAX] =
{
		[eSUCCESS_STRING] = "Success",
		[eFAILURE_STRING] = "Failure"
};

/**
 * @brief convert eDeviceResetCause_t enum to strings for printing
 *
 */
__attribute__((unused)) static const char* gcResetCauseEnumToStringHelper[eRESET_CAUSE_MAX] =
{
	    [eRESET_CAUSE_UNKNOWN] 						= "UNKNOWN",
		[eRESET_CAUSE_WAKEUP]						= "SLEEP_COMPLETE",
	    [eRESET_CAUSE_LOW_POWER_RESET] 				= "LOW_POWER_RESET",
	    [eRESET_CAUSE_WINDOW_WATCHDOG_RESET] 		= "WWDG_RESET",
	    [eRESET_CAUSE_INDEPENDENT_WATCHDOG_RESET] 	= "IWDG_RESET",
	    [eRESET_CAUSE_SOFTWARE_RESET] 				= "SOFTWARE_RESET",
	    [eRESET_CAUSE_POWER_ON_POWER_DOWN_RESET] 	= "POWER_CYCLE",
	    [eRESET_CAUSE_EXTERNAL_RESET_PIN_RESET] 	= "EXTERNAL_RESET",
	    [eRESET_CAUSE_BROWNOUT_RESET] 				= "BROWN_OUT_RESET",
};

///////////////////////////////////////////////////////////////////////////////


/**
 * @brief Function to be invoked prior to resetting the system to recover from an hard fault/error
 *
 */
static void AppCommon_PreResetRoutine()
{
	/**< Device has been reset so set up-time back to Zero*/
	__NOP();
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Setter for @ref gErrorCode, previous values are unaffected
 *
 * @return eDeviceErrorCode_t
 */
eDeviceErrorCode_t AppCommon_GetErrorCode()
{
	return gErrorCode;
}

/**
 * @brief Setter for @ref gErrorCode, previous values are unaffected
 *
 */
void AppCommon_AccumlateErrorCode(eDeviceErrorCode_t err)
{
	gErrorCode |= err;

}

/**
 * @brief Reset error Code
 *
 */
void AppCommon_ResetErrorCode()
{
	gErrorCode = eERR_NO_ERRORS;

}

/**
 * @brief Function to determine why the device was reset, also sets global @ref gDeviceResetCause
 * @note should be called before peripheral initialization
 *
 */
void AppCommon_DetermineResetCause()
{
	eDeviceResetCause_t resetCause = eRESET_CAUSE_UNKNOWN;

	if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        resetCause = eRESET_CAUSE_LOW_POWER_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        resetCause = eRESET_CAUSE_WINDOW_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        resetCause = eRESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        resetCause = eRESET_CAUSE_SOFTWARE_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        resetCause = eRESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        resetCause = eRESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }
    else if(SET == __HAL_PWR_GET_FLAG(PWR_FLAG_SB))
	{
    	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU|PWR_FLAG_SB);

        resetCause = eRESET_CAUSE_WAKEUP;
	}
    else
    {
        resetCause = eRESET_CAUSE_UNKNOWN;
    }

    /**< Clear all the reset flags or else they will remain set during future resets until system power is fully removed. */

    __HAL_RCC_CLEAR_RESET_FLAGS();

    gDeviceResetCause = resetCause;

    return;

}

/**
 * @brief Getter for @ref gDeviceResetCause
 * @note Assumes @ref AppCommon_DetermineResetCause is called at startup
 * @return eDeviceResetCause_t
 *
 */
eDeviceResetCause_t AppCommon_GetCachedResetCause()
{
	return gDeviceResetCause;
}


/**
 * @brief STM HAL error handler
 * @warning This function resets the device
 *
 */
void AppCommon_HALErrorHandler()
{
	__disable_irq();
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> ERROR: HAL error handler invoked");

	AppIndicate_SetState(eIND_RED_250MS);

	AppCommon_PreResetRoutine();

#ifdef NDEBUG
	HAL_NVIC_SystemReset();
#endif
	LOOP_FOREVER
}

/**
 * @brief STM Fault error handler
 * @warning This function resets the device
 *
 */
void AppCommon_STMFaultHandler()
{
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> ERROR: ARM Fault Encountered");

	AppIndicate_SetState(eIND_RED_250MS);

	AppCommon_PreResetRoutine();
#ifdef NDEBUG
	HAL_NVIC_SystemReset();
#endif
	LOOP_FOREVER
}

/**
 * @brief Assert failure error handler
 * @warning This function resets the device
 *
 */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> ERROR: Assertion failure in file %s on line %d at function %s", file, line, func);

	AppIndicate_SetState(eIND_RED_250MS);

	AppCommon_PreResetRoutine();
#ifdef NDEBUG
	HAL_NVIC_SystemReset();
#endif
	LOOP_FOREVER
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Utility function to print delimiter used in the console
 *
 */
void AppCommon_PrintDelimiter()
{
	const char cDelimiterLine[] = "\r\n\r\n************************************************************\r\n";
	Console_Print(eCONSOLE_PRINT_LVL0, "%s",cDelimiterLine);
}

/**
 * @brief Utility function to print delimiter used in the console
 *
 */
void AppCommon_PrintLineBreak()
{
	const char cDelimiterLine2[] = "\r\n";
	Console_Print(eCONSOLE_PRINT_LVL0, "%s",cDelimiterLine2);
}

/**
 * @brief Start up message print
 *
 */
void AppCommon_StartUpMessagePrint()
{

	AppCommon_PrintDelimiter();
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Fasal. Grow More, Grow Better!\r\n Wolkus Technology Solutions Private Limited, Bangalore, India.");
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Device: HW version: %d.%d, FW version: %d.%d", HARDWARE_VERSION_MAJOR, HARDWARE_VERSION_MINOR, FW_VERSION_MAJOR, FW_VERSION_MINOR);
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Binaries compiled on %s at %s", COMPILE_DATE, COMPILE_TIME );
	Console_Print(eCONSOLE_PRINT_LVL0, "\r\n Build for Fasal Flasher Board");
	AppCommon_PrintDelimiter();
}

/**
 * @brief Utility function returns "Success" or "failure" based on status code passed. @see gcStatusStringHelper
 *
 * @param status 0 is success across all modules in the code-base , non 0 is treated as failure
 * @return const char* const
 */
const char* const AppCommon_GetStatusString(int status)
{
	eStatusPrintHelperEnum_t statusStringEnum = (0 == status)?eSUCCESS_STRING: eFAILURE_STRING;

	return gcStatusStringHelper[statusStringEnum];

}

///////////////////////////////////////////////////////////////////////////////
