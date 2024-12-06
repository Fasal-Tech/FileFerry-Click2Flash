/**
 * @file AppInterrupts.c
 * @author Vishal Keshava Murthy
 * @brief Application Interrupt callbacks defines
 * @version 0.1
 * @date 2024-10-24
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "AppConfiguration.h"
#include "AppIndication.h"
#include "AppInterrupts.h"
#include "Console.h"
#include "softTimer.h"
#include "usart.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function to be invoked prior to resetting the system to recover from an hard fault/error
 *
 */
static void AppISR_PreResetRoutine()
{
    __NOP();
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Error from STM HAL
 * @warning This function resets the device
 *
 */
void AppISR_HALErrorHandler()
{
    __disable_irq();
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> ERROR: HAL error handler invoked");

    AppIndicate_SetState(eIND_RED_250MS);

    AppISR_PreResetRoutine();

#ifdef NDEBUG
    HAL_NVIC_SystemReset();
#endif
    LOOP_FOREVER
}

/**
 * @brief Handler for errors from ARM core
 * @warning This function resets the device
 *
 */
void AppISR_ARMHandler()
{
    __disable_irq();
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> ERROR: ARM Fault Encountered");

    AppIndicate_SetState(eIND_RED_250MS);

    AppISR_PreResetRoutine();

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
void __assert_func(const char* file, int line, const char* func, const char* failedexpr)
{
    Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> ERROR: Assertion failure in file %s on line %d at function %s", file, line, func);

    AppIndicate_SetState(eIND_RED_250MS);

    AppISR_PreResetRoutine();

#ifdef NDEBUG
    HAL_NVIC_SystemReset();
#endif
    LOOP_FOREVER
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief STM HAL reception callback
 *
 * @param huart
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == (SOFTTIMER_TIMER_INSTANCE)->Instance)
    {
        SoftTimer_cbPeriodicCheck();
    }
}
