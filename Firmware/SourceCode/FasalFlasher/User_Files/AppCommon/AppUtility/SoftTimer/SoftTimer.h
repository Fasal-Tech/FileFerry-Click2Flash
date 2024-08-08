/**
 * @file SoftTimer.h
 * @author Vishal Keshava Murthy 
 * @brief soft-timer Interface
 * @version 0.2
 * @date 2023-05-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_SOFTTIMER_SOFTTIMER_H_
#define UTILITY_SOFTTIMER_SOFTTIMER_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "tim.h"

///////////////////////////////////////////////////////////////////////////////

#define SOFTTIMER_TIMER_INSTANCE 		(&htim6)
#define SOFTTIMER_IRQ					(TIM6_IRQn)
#define SOFTTIMER_OVERFLOW_PERIOD_MS	(10)

///////////////////////////////////////////////////////////////////////////////

typedef void (*pfCallBack_t)(void);

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief soft-timer IDs
 * 
 */
typedef enum
{
	eGENERIC_COUNT_DOWN_TIMER,	/**< Generic timer for timeout checks*/
	ePUSH_BUTTON_TIMER,
	eDEBUG_LED_SOFT_TIMER,
	eSOFT_TIMER_MAX
}eSoftTimerID_t;

/**
 * @brief Soft-timer structure
 * 
 */
typedef struct
{
	bool IsArmed;
	bool IsPeriodic;
	uint32_t currentTicks;
	uint32_t setTicks;
	pfCallBack_t pfCallBack;
}sSoftTimer_t;

///////////////////////////////////////////////////////////////////////////////

void SoftTimer_Register(eSoftTimerID_t id, uint32_t timeOut, bool IsPeriodic, pfCallBack_t callbackFunction );
void SoftTimer_Init();
void SoftTimer_DeInit();
void SoftTimer_Start(eSoftTimerID_t id, bool IsStartNeeded);
void SoftTimer_Pause(eSoftTimerID_t id, bool IsPauseNeeded);
void SoftTimer_DelayMS(uint32_t delayMS);

void SoftTimer_AperiodicTimerSet(uint32_t timeOut);
bool SoftTimer_HasAperiodicTimerExpired();

void SoftTimer_cbPeriodicCheck();



///////////////////////////////////////////////////////////////////////////////


#endif /* UTILITY_SOFTTIMER_SOFTTIMER_H_ */
