/**
 * @file CommonInterrupts.c
 * @author Vishal Keshava Murthy
 * @brief All Interrupt routines shared across modules are implemented here
 * @version 0.1
 * @date 2023-05-28
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "usart.h"

#include "Console.h"
#include "softTimer.h"

#include "CommonInterrupts.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief STM HAL reception callback
 *
 * @param huart
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
    if (huart == &huart1)
    {
    	Console_cbCommandReceived();
    }
}


/**
 * @brief Timer callback associated with soft-timer, part of STM HAL
 *
 * @param htim timer instance
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == (SOFTTIMER_TIMER_INSTANCE)->Instance)
    {
    	SoftTimer_cbPeriodicCheck();
    }
}

