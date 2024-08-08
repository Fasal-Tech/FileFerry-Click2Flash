/**
 * @file AppFasal.h
 * @author Vishal Keshava Murthy
 * @brief Fasal Application Interface
 * @version 0.1
 * @date 2023-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPFASAL_APPFASAL_H_
#define APPFASAL_APPFASAL_H_

///////////////////////////////////////////////////////////////////////////////

#include "AppCommon.h"

///////////////////////////////////////////////////////////////////////////////


/**
 * @brief Enumeration for various states of @ref AppFasal_RunNova
 *
 */
typedef enum
{
	eFASAL_APP_INIT,
	eFASAL_APP_STARTUP_MSG,
	eFASAL_APP_BUTTON_WAIT,
	eFASAL_APP_SD_INIT,
	eFASAL_APP_SD_CHECK,
	eFASAL_APP_FLASH_INIT,
	eFASAL_APP_MODE_SELECTION,
	eFASAL_APP_SD_FLASH_TRANSFER,
	eFASAL_APP_XMODEM_TRANSFER,
	eFASAL_APP_CRC_COMPARE,
	eFASAL_APP_TRANSFER_SUCCESS,

	eFASAL_APP_SD_FAIL,
	eFASAL_APP_SD_FILE_FAIL,
	eFASAL_APP_FLASH_FAIL,
	eFASAL_APP_TRANSFER_FAIL,
	eFASAL_APP_CRC_FAIL,

	eFASAL_APP_END,

	eFASAL_MAX_STATE,
}eAppFasalStates_t;

///////////////////////////////////////////////////////////////////////////////

eAppFasalStates_t AppFasal_Run();

///////////////////////////////////////////////////////////////////////////////

#endif /* APPFASAL_APPFASAL_H_ */
