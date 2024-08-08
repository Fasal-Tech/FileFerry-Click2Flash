/**
 * @file SoftTimer.c
 * @author Vishal Keshava Murthy
 * @brief soft-timer implementation
 * @version 0.2
 * @date 2023-05-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "SoftTimer/SoftTimer.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief All soft-timer instances are captured here
 * 
 */
static volatile sSoftTimer_t gvSoftTimers[eSOFT_TIMER_MAX] ;


///////////////////////////////////////////////////////////////////////////////



/**
 * @brief Periodic call from soft-timer that checks if any registered timers have expired
 * and invokes the registered call back @ref HAL_TIM_PeriodElapsedCallback
 *
 */
void SoftTimer_cbPeriodicCheck()
{
	for (eSoftTimerID_t i = 0; i < eSOFT_TIMER_MAX; i++)
	{
		if (!gvSoftTimers[i].IsArmed || gvSoftTimers[i].currentTicks == 0)
		{
			continue;
		}

		if (--gvSoftTimers[i].currentTicks == 0)
		{
			if(NULL != gvSoftTimers[i].pfCallBack )
			{
				gvSoftTimers[i].pfCallBack();
			}

			if (gvSoftTimers[i].IsPeriodic)
			{
				gvSoftTimers[i].currentTicks = gvSoftTimers[i].setTicks;
				gvSoftTimers[i].IsArmed = true;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Default call back for soft-timers without any executors
 * 
 */
static void SoftTimer_DefaultCallBackFunction()
{
	__NOP();
}

/**
 * @brief Utility function to convert timeinMS to soft-timer ticks count
 * 
 * @param timeMs 
 * @return uint32_t
 */
static uint32_t SoftTimer_timeToTicks(uint32_t timeMs)
{
	uint32_t ticks = timeMs/SOFTTIMER_OVERFLOW_PERIOD_MS;

	return ticks;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize soft-timer
 * 
 */
void SoftTimer_Init()
{
	for(eSoftTimerID_t i=0; i<eSOFT_TIMER_MAX; i++)
	{
		gvSoftTimers[i].IsArmed = false;
		gvSoftTimers[i].IsPeriodic = false;
		gvSoftTimers[i].pfCallBack = SoftTimer_DefaultCallBackFunction;
	}

	HAL_TIM_Base_Start_IT(SOFTTIMER_TIMER_INSTANCE);
}

/**
 * @brief Get time left for expiry
 *
 * @param id ID of soft-timer
 * @return uint32_t time left for soft-timer expire in ms
 */
__attribute__((unused)) static uint32_t SoftTimer_GetTimeLeft(eSoftTimerID_t id)
{
	assert(eSOFT_TIMER_MAX > id);

	HAL_NVIC_DisableIRQ(SOFTTIMER_IRQ);

	uint32_t timeLeft = ((gvSoftTimers[id].currentTicks)*SOFTTIMER_OVERFLOW_PERIOD_MS) ;

	HAL_NVIC_EnableIRQ(SOFTTIMER_IRQ);

	return timeLeft;
}

/**
 * @brief DeInitialize soft-timer
 *
 */
void SoftTimer_DeInit()
{
	HAL_TIM_Base_Stop_IT(SOFTTIMER_TIMER_INSTANCE);
}

/**
 * @brief Register a periodic function with soft-timer, create ID under @ref eSoftTimerID
 * and attach call-back and time period through this function
 * 
 * @param id 
 * @param timeOut 
 * @param IsPeriodic
 * @param callbackFunction 
 */
void SoftTimer_Register(eSoftTimerID_t id, uint32_t timeOut, bool IsPeriodic, pfCallBack_t callbackFunction )
{
	assert(id < eSOFT_TIMER_MAX);

	callbackFunction = (NULL == callbackFunction)? SoftTimer_DefaultCallBackFunction : callbackFunction;

	HAL_NVIC_DisableIRQ(SOFTTIMER_IRQ);

	gvSoftTimers[id].IsArmed = false;
	gvSoftTimers[id].IsPeriodic = IsPeriodic;
	gvSoftTimers[id].pfCallBack = callbackFunction;
	gvSoftTimers[id].setTicks = SoftTimer_timeToTicks(timeOut);
	gvSoftTimers[id].currentTicks = SoftTimer_timeToTicks(timeOut);

	HAL_NVIC_EnableIRQ(SOFTTIMER_IRQ);

}

/**
 * @brief Start soft-timer
 * 
 * @param id 
 * @param IsStartNeeded
 */
void SoftTimer_Start(eSoftTimerID_t id, bool IsStartNeeded)
{
	assert(id < eSOFT_TIMER_MAX);

	HAL_NVIC_DisableIRQ(SOFTTIMER_IRQ);

	gvSoftTimers[id].IsArmed = IsStartNeeded;
	gvSoftTimers[id].currentTicks = gvSoftTimers[id].setTicks ;


	HAL_NVIC_EnableIRQ(SOFTTIMER_IRQ);
}


/**
 * @brief Function to disable and enable soft-timer for creating critical sections
 * 
 * @param id ID of the soft-timer to pause/resume
 * @param IsPauseNeeded Pause if true, resume if false
 */
void SoftTimer_Pause(eSoftTimerID_t id, bool IsPauseNeeded)
{
	assert(id < eSOFT_TIMER_MAX);

	HAL_NVIC_DisableIRQ(SOFTTIMER_IRQ);

	gvSoftTimers[id].IsArmed = (true == IsPauseNeeded)?false:true;

	HAL_NVIC_EnableIRQ(SOFTTIMER_IRQ);
}

/**
 * @brief Collectively start all registered soft-timers
 * 
 * @param IsStartNeeded
 */
void SoftTimer_StartAll(bool IsStartNeeded)
{
	for(eSoftTimerID_t i=0; i<eSOFT_TIMER_MAX; i++)
	{
		SoftTimer_Start(i, IsStartNeeded);
	}
}

/**
 * @brief Check if soft-timers  has expired
 * 
 * @param id id of soft-timer  to be checked
 * @return true if soft-timer has expired
 * @return false if V has not expired / has not been enabled
 */
bool SofTimer_IsExpired(eSoftTimerID_t id)
{
	assert(id < eSOFT_TIMER_MAX);

	return (0 == gvSoftTimers[id].currentTicks);
}

/**
 * @brief Set Aperiodic timer instance useful for setting timeouts 
 * @note @ref eGENERIC_COUNT_DOWN_TIMER must not be used for other periodic timers 
 * 
 * @param timeOut time out to set the aperiodic timer
 */
void SoftTimer_AperiodicTimerSet(uint32_t timeOut)
{
	SoftTimer_Register(eGENERIC_COUNT_DOWN_TIMER, timeOut, false, SoftTimer_DefaultCallBackFunction);
	SoftTimer_Start(eGENERIC_COUNT_DOWN_TIMER, true);
}

/**
 * @brief Check if aperiodic timer has expired
 * 
 * @return true if timer has expired 
 * @return false if timer has not expired/ Registered / started 
 */
bool SoftTimer_HasAperiodicTimerExpired()
{
	return SofTimer_IsExpired(eGENERIC_COUNT_DOWN_TIMER);
}

/**
 * @brief Wrapper around HAL_Delay delay
 *
 * @param delayMS delay needed in mS
 */
void SoftTimer_DelayMS(uint32_t delayMS)
{
    HAL_Delay(delayMS);
}
