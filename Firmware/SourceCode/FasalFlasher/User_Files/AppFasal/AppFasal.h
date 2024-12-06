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

/**
 * @brief Enumeration for various states of @ref AppFasal_RunNova
 *
 */
typedef enum
{
    eAPP_INIT,
    eAPP_STARTUP_MSG,
    eAPP_BUTTON_WAIT,
    eAPP_SD_INIT,
    eAPP_SD_CHECK,
    eAPP_FLASH_INIT,
    eAPP_MODE_SELECTION,
    eAPP_SD_FLASH_TRANSFER,
    eAPP_XMODEM_TRANSFER,
    eAPP_CRC_COMPARE,
    eAPP_TRANSFER_SUCCESS,
    eAPP_SD_FAIL,
    eAPP_SD_FILE_FAIL,
    eAPP_FLASH_FAIL,
    eAPP_TRANSFER_FAIL,
    eAPP_CRC_FAIL,
    eAPP_END,
    eAPP_MAX_STATE,
} eAppFasalStates_t;

///////////////////////////////////////////////////////////////////////////////

eAppFasalStates_t AppFasal_Run();

///////////////////////////////////////////////////////////////////////////////

#endif /* APPFASAL_APPFASAL_H_ */
