/**
 * @file Console.c
 * @author Vishal Keshava Murthy
 * @brief Console module implementation
 * @version 0.1
 * @date 2023-05-24
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "usart.h"

#include "Console.h"
#include "SoftTimer.h"
#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

static char gDataBuffer[CONSOLE_BUFFER_SIZE] = {0};		/**< Global buffer used by Console print*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Console commands, names and their flags are stored in this table
 *
 */
static sConsoleCommand_t gConsoleCommandHelperTable[eCONSOLE_MAX_COMMANDS] =
{
};

///////////////////////////////////////////////////////////////////////////////

static volatile sElevatedPromptData_t gvElevatedPromptData = {.buf = {0}};  /**< Buffer for saving elevated prompt data */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Call back function invoked by @ref HAL_UART_RxCpltCallback upon receiving @ref CONSOLE_COMMAND_TOKEN_SIZE characters
 * @note CONSOLE_COMMAND_TOKEN_SIZE is set to strlen(gConsoleCommandHelperTable[eCONSOLE_LEVEL2_ENABLE].CommandStr) when expecting Secret key for level 2 logs
 *
 */
void Console_cbCommandReceived()
{
	for(int i=0; i<eCONSOLE_MAX_COMMANDS; i++)
	{
		/**< Check if required passkey is received, if not continue to listen on the console Rx*/
    	if(NULL != strstr((const char*)gvElevatedPromptData.buf , gConsoleCommandHelperTable[i].CommandStr) )
    	{
    		memset(((char*)gvElevatedPromptData.buf), 0, sizeof(gvElevatedPromptData.buf));
    		Console_RaiseConsoleCmdRequest(i);
            break;
    	}
	}
	HAL_UART_Receive_IT(CONSOLE_UART_HANDLE, (uint8_t*)gvElevatedPromptData.buf, CONSOLE_COMMAND_TOKEN_SIZE);
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize console
 *
 */
void Console_Init()
{
	HAL_UART_Receive_IT(CONSOLE_UART_HANDLE, (uint8_t*)gvElevatedPromptData.buf, CONSOLE_COMMAND_TOKEN_SIZE);
}

/**
 * @brief DeInitialize console
 *
 */
void Console_DeInit()
{
	HAL_UART_Abort(CONSOLE_UART_HANDLE);
	HAL_UART_MspDeInit(CONSOLE_UART_HANDLE);
}

/**
 * @brief   Transmit character over Console
 * @param   data: char to be transmitted
 * @return  eConsoleStatus_t
 */
eConsolePrintStatus_t Console_TransmitChar(uint8_t data)
{
	eConsolePrintStatus_t status = eCONSOLE_FAIL;

	/* Make available the UART module. */
	if (HAL_UART_STATE_TIMEOUT == HAL_UART_GetState(CONSOLE_UART_HANDLE))
	{
		HAL_UART_Abort(CONSOLE_UART_HANDLE);
	}

	if (HAL_OK == HAL_UART_Transmit(CONSOLE_UART_HANDLE, &data, 1u, CONSOLE_UART_TIMEOUT_MS*10u))
	{
		status = eCONSOLE_SUCCESS;
	}
	return status;
}

/**
 * @brief Console print function
 *
 * @param currentLevel : What level to print the logs in
 * @param format format string
 * @param ...
 * @return eConsolePrintStatus_t
 */
eConsolePrintStatus_t Console_Print(eConsolePrintLevel_t currentLevel, char *format,...)
{
	assert(currentLevel < eCONSOLE_PRINT_LVL_MAX);

	eConsolePrintStatus_t status = eCONSOLE_SUCCESS;

	bool IsHardwareOverRideActive = false;	/**< not supported in this Hardware*/

	bool PrintLevelSufficient = (
									(eCONSOLE_PRINT_LVL0 == currentLevel) ||
									((eCONSOLE_PRINT_LVL1 == currentLevel) && (true == gConsoleCommandHelperTable[eCONSOLE_LEVEL1_ENABLE].IsCommandRaised)) ||
									((eCONSOLE_PRINT_LVL2 == currentLevel) && (true == gConsoleCommandHelperTable[eCONSOLE_LEVEL2_ENABLE].IsCommandRaised))
								);

	/**< For print to get through, print level should be sufficient, Hardware override works on levels below @ref eCONSOLE_PRINT_LVL2*/
	if((true == PrintLevelSufficient) || ((true == IsHardwareOverRideActive) && (eCONSOLE_PRINT_LVL2 != currentLevel)))
	{
		memset(gDataBuffer, 0, sizeof(gDataBuffer));

		va_list args;
		va_start(args, format);
		int formattedLength = vsnprintf(gDataBuffer, sizeof(gDataBuffer), format, args);
		va_end(args);

		if(HAL_OK != HAL_UART_Transmit(CONSOLE_UART_HANDLE, (uint8_t*)gDataBuffer, formattedLength , CONSOLE_UART_TIMEOUT_MS))
		{
			status = eCONSOLE_FAIL;
		}
	}

	return status;
}

/**
 * @brief   Receive data over console
 * @param   pOutdata Received data is saved here
 * @param   length:  Size of data read
 * @return  eConsoleStatus_t
 */
eConsolePrintStatus_t Console_receive(uint8_t* pOutdata, uint16_t length)
{
	eConsolePrintStatus_t status = eCONSOLE_FAIL;

	/* Make available the UART module. */
	if (HAL_UART_STATE_READY != HAL_UART_GetState(CONSOLE_UART_HANDLE))
	{
		HAL_UART_Abort(CONSOLE_UART_HANDLE);
	}

	HAL_StatusTypeDef HALStatus =  HAL_UART_Receive(CONSOLE_UART_HANDLE, pOutdata, length, CONSOLE_UART_TIMEOUT_MS);

	status = (HAL_OK == HALStatus)? eCONSOLE_SUCCESS: eCONSOLE_FAIL;

	return status;
}


/**
 * @brief Thin wrapper around UART transmit for Progress Bar print on console
 *
 */
void Console_PrintProgressBar()
{
	const char cPROGRESS_BAR[] = ".";
	HAL_UART_Transmit(CONSOLE_UART_HANDLE, (uint8_t*)cPROGRESS_BAR, strlen(cPROGRESS_BAR) , 0);
}


/**
 * @brief Utility function to check if any command request has been made
 *
 * @param cmd
 * @return true
 * @return false
 */
bool Console_IsCommandRaised(eConsoleCommandsEnum_t cmd)
{
	assert(cmd < eCONSOLE_MAX_COMMANDS);

	return (gConsoleCommandHelperTable[cmd].IsCommandRaised);

}

/**
 * @brief Raise a request to process a console command
 *
 */
void Console_RaiseConsoleCmdRequest(eConsoleCommandsEnum_t cmd)
{
	if(cmd < eCONSOLE_MAX_COMMANDS)
	{
		gConsoleCommandHelperTable[cmd].IsCommandRaised = true;
		HAL_UART_Transmit(CONSOLE_UART_HANDLE, (uint8_t*)&cmd, sizeof(cmd), 0);
	}
}

/**
 * @brief Deferred processing of commands received over console
 *
 */
void Console_Sync()
{
    for(int i = 0; i<eCONSOLE_MAX_COMMANDS; i++ )
    {
        if((true == gConsoleCommandHelperTable[i].IsCommandRaised) && (false == gConsoleCommandHelperTable[i].IsCommandServiced) )
        {
            if(NULL != gConsoleCommandHelperTable[i].pfConsoleCommandActor)
            {
                gConsoleCommandHelperTable[i].IsCommandServiced = true;
                gConsoleCommandHelperTable[i].pfConsoleCommandActor() ;
            }
        }

    }
}

