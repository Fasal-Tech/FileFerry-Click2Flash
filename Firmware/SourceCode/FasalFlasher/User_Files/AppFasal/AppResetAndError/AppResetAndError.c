/**
 * @file AppResetAndError.h
 * @author Vishal Keshava Murthy
 * @brief Reset and error handling Interface
 * @version 0.1
 * @date 2024-10-24
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "AppResetAndError.h"
#include "AppConfiguration.h"
#include "AppIndication.h"
#include "Console.h"

///////////////////////////////////////////////////////////////////////////////

///< Global that maintains error cause
static eDeviceErrorCode_t gErrorCode = eERR_NO_ERRORS;

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
 * @brief Utility function returns "Success" or "failure" based on status code passed. @see gcStatusStringHelper
 *
 * @param status 0 is success across all modules in the code-base , non 0 is treated as failure
 * @return const char* const
 */
const char* const AppCommon_GetStatusString(int status)
{
    const char* statusString = (0 == status) ? "Success" : "Failure";

    return statusString;
}

///////////////////////////////////////////////////////////////////////////////
