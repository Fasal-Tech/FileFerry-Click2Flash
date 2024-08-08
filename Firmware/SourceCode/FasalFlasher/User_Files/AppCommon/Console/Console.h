/**
 * @file Console.h
 * @author Vishal Keshava Murthy
 * @brief Console module interface
 * @version 0.1
 * @date 2023-05-24
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "Utility.h"

///////////////////////////////////////////////////////////////////////////////


/**
 * @brief Console buffer size, To be increased if larger strings are to be printed
 *
 */
#define CONSOLE_BUFFER_SIZE		    (512u)	/**< Maximum expected response is just under 200 bytes*/
#define CONSOLE_UART_TIMEOUT_MS     (1000u)
#define CONSOLE_PROMPT_BUFFER_SIZE  (8u)
#define CONSOLE_COMMAND_TOKEN_SIZE	(3u)

#define CONSOLE_UART_HANDLE			(&huart1)

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Console command handlers
 *
 */
typedef void (*pfConsoleCommandActor_t)(void);

/**
 * @brief Enum that maintains Console status
 *
 */
typedef enum
{
	eCONSOLE_SUCCESS,
	eCONSOLE_FAIL,
	eCONSOLE_STATUS_MAX
}eConsolePrintStatus_t;

/**
 * @brief Command names are enumerated here
 *
 */
typedef enum
{
    eCONSOLE_LEVEL1_ENABLE,
    eCONSOLE_LEVEL2_REQUEST,
    eCONSOLE_LEVEL2_ENABLE,
    eCONSOLE_ERASE_FLASH_REQUEST,
	eCONSOLE_SENSOR_TEST_REQUEST,
    eCONSOLE_MAX_COMMANDS
}eConsoleCommandsEnum_t;

/**
 * @brief Console print level
 *
 */
typedef enum
{
	eCONSOLE_PRINT_LVL0,
	eCONSOLE_PRINT_LVL1,
	eCONSOLE_PRINT_LVL2,
	eCONSOLE_PRINT_LVL_MAX,
}eConsolePrintLevel_t;

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Prompt structure
 *
 */
typedef struct
{
	char buf[CONSOLE_PROMPT_BUFFER_SIZE];
}sElevatedPromptData_t;

/**
 * @brief Helper structure used by console to process incoming commands
 *
 */
typedef struct
{
    const char* CommandName;
    char CommandStr[BUFFER_SIZE_8];
    volatile bool IsCommandRaised;
    volatile bool IsCommandServiced;
    pfConsoleCommandActor_t pfConsoleCommandActor;
}sConsoleCommand_t;

///////////////////////////////////////////////////////////////////////////////

void Console_Init();
void Console_DeInit();
eConsolePrintStatus_t Console_TransmitChar(uint8_t data);
eConsolePrintStatus_t Console_Print(eConsolePrintLevel_t currentLevel, char *format,...);
eConsolePrintStatus_t Console_receive(uint8_t* pOutdata, uint16_t length);
void Console_cbCommandReceived();
bool Console_IsCommandRaised(eConsoleCommandsEnum_t cmd);
void Console_RaiseConsoleCmdRequest(eConsoleCommandsEnum_t cmd);
void Console_Sync();
void Console_PrintProgressBar();

///////////////////////////////////////////////////////////////////////////////

#endif /* CONSOLE_CONSOLE_H_ */
