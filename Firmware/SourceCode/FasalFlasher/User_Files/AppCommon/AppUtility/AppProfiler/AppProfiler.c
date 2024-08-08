/**
 * @file AppProfiler.c
 * @author Vishal Keshava Murthy
 * @brief Utility implementation to measure function execution time based on DWT module
 * @version 0.1
 * @date 2024-06-13
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "AppProfiler.h"
#include "stm32f1xx_hal.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Start measurement of function execution time
 */
void AppProfiler_StartExecutionTimeMeasurement()
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
 * @brief Get function execution time.
 * @note This function expects @ref AppProfiler_StartExecutionTimeMeasurement to be invoked prior to use
 *
 * @return execution time in milliseconds
 */
uint32_t AppProfiler_GetExecutionTimeMS()
{
	uint32_t currentTick = DWT->CYCCNT;

	uint32_t executionTimeMs = currentTick / (SystemCoreClock / 1000u) ;

	return executionTimeMs;
}

///////////////////////////////////////////////////////////////////////////////
