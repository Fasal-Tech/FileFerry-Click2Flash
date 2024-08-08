/**
 * @file TriColorLED.c
 * @author Vishal Keshava Murthy
 * @brief Tri-Color LED Implementation
 * @version 0.1
 * @date 2023-09-28
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "TriColorLED.h"
#include "SoftTimer.h"

///////////////////////////////////////////////////////////////////////////////


/**
 * @brief Helper table that maps LED state to GPIO state
 *
 */
static const GPIO_PinState gcLEDEnumtoGPIOStateConverter[eLED_STATE_MAX] =
{
		[eLED_OFF] = GPIO_PIN_RESET,
		[eLED_ON] = GPIO_PIN_SET,
};

/**
 * @brief LED pin mapping helper table
 */
static const sLEDPins_t gcLEDPins[eLED_MAX] =
{
		[eLED_RED] 		= {.LEDPort = LED_R_GPIO_Port, .LEDPin = LED_R_Pin },
		[eLED_GREEN]	= {.LEDPort = LED_G_GPIO_Port, .LEDPin = LED_G_Pin },
		[eLED_BLUE]		= {.LEDPort = LED_B_GPIO_Port, .LEDPin = LED_B_Pin },
};

/**
 * @brief LED pin mapping helper table
 *  * warning eLED_BLUE and eLED_RED are swapped in Rev 1A Hardware
 */
static const sLEDPins_t gcLEDPins2[eLED_MAX] =
{
		[eLED_BLUE] 	= {.LEDPort = LED_R1_GPIO_Port, .LEDPin = LED_R1_Pin },
		[eLED_GREEN]	= {.LEDPort = LED_G1_GPIO_Port, .LEDPin = LED_G1_Pin },
		[eLED_RED]		= {.LEDPort = LED_B1_GPIO_Port, .LEDPin = LED_B1_Pin },
};

/**
 * @brief Helper structure to map TriColor LED enumerations to Individual LED combinations
 *
 */
static const sTriColorLEDState_t gcLEDStateHelperTable[eLED_COLOR_STATE_MAX] =
{
		[eLED_COLOR_ALL_OFF].LedState =
		{
				[eLED_RED] 		= eLED_OFF,
				[eLED_GREEN] 	= eLED_OFF,
				[eLED_BLUE]		= eLED_OFF,
		},

		[eLED_COLOR_RED].LedState =
		{
				[eLED_RED] 		= eLED_ON,
				[eLED_GREEN] 	= eLED_OFF,
				[eLED_BLUE]		= eLED_OFF,
		},

		[eLED_COLOR_GREEN].LedState =
		{
				[eLED_RED] 		= eLED_OFF,
				[eLED_GREEN] 	= eLED_ON,
				[eLED_BLUE]		= eLED_OFF,
		},

		[eLED_COLOR_BLUE].LedState =
		{
				[eLED_RED] 		= eLED_OFF,
				[eLED_GREEN] 	= eLED_OFF,
				[eLED_BLUE]		= eLED_ON,

		},

		[eLED_COLOR_YELLOW].LedState =
		{
				[eLED_RED] 		= eLED_ON,
				[eLED_GREEN] 	= eLED_ON,
				[eLED_BLUE]		= eLED_OFF,

		},

		[eLED_COLOR_MAGENTA].LedState =
		{
				[eLED_RED] 		= eLED_ON,
				[eLED_GREEN] 	= eLED_OFF,
				[eLED_BLUE]		= eLED_ON,

		},

		[eLED_COLOR_CYAN].LedState =
		{
				[eLED_RED] 		= eLED_OFF,
				[eLED_GREEN] 	= eLED_ON,
				[eLED_BLUE]		= eLED_ON,

		},

		[eLED_COLOR_WHITE].LedState =
		{
				[eLED_RED] 		= eLED_ON,
				[eLED_GREEN] 	= eLED_ON,
				[eLED_BLUE]		= eLED_ON,

		},
};

/**
 * @brief Utility table that saves LED timings to ticks based on @ref TRICOLOR_LED_BLINK_TIME_BASE_MS
 *
 */
static const uint32_t gcBlinkPeriodToCountHelper[eLED_BLINK_PERIOD_MAX] =
{
		[eTRICOLORLED_BLINK_NONE]		= UINT32_MAX,
		[eTRICOLORLED_BLINK_250MS] 		= 250/TRICOLOR_LED_BLINK_TIME_BASE_MS,
		[eTRICOLORLED_BLINK_500MS] 		= 500/TRICOLOR_LED_BLINK_TIME_BASE_MS,
		[eTRICOLORLED_BLINK_1000MS] 	= 1000/TRICOLOR_LED_BLINK_TIME_BASE_MS,
		[eTRICOLORLED_BLINK_2000MS] 	= 2000/TRICOLOR_LED_BLINK_TIME_BASE_MS
};

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Tricolor LED instance
 *
 */
static volatile sTricolorLED_t gvTricolorLed =
{
		.Leds[eLED_RED] =
		{
			.pcLedPin = &gcLEDPins[eLED_RED],
			.ledState= eLED_OFF
		},

		.Leds[eLED_GREEN] =
		{
			.pcLedPin = &gcLEDPins[eLED_GREEN],
			.ledState= eLED_OFF
		},

		.Leds[eLED_BLUE] =
		{
			.pcLedPin = &gcLEDPins[eLED_BLUE],
			.ledState= eLED_OFF
		},

		.Leds2[eLED_RED] =
		{
			.pcLedPin = &gcLEDPins2[eLED_RED],
			.ledState= eLED_OFF
		},

		.Leds2[eLED_GREEN] =
		{
			.pcLedPin = &gcLEDPins2[eLED_GREEN],
			.ledState= eLED_OFF
		},

		.Leds2[eLED_BLUE] =
		{
			.pcLedPin = &gcLEDPins2[eLED_BLUE],
			.ledState= eLED_OFF
		},

};

///////////////////////////////////////////////////////////////////////////////

static void TriColorLed_ToggleState(volatile sTricolorLED_t* const pMe);

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get tricolor LED instance
 *
 * @return volatile* const
 */
static volatile sTricolorLED_t* const TriColorLed_GetInstance()
{
	return &gvTricolorLed;
}

/**
 * @brief Call back function registered with softtimer
 *
 */
static void TriColorLED_cbBlinkHandler()
{
	volatile sTricolorLED_t* const pMe = TriColorLed_GetInstance();

	if((true == pMe->IsInitialized) && (true == pMe->IsBlinkEnabled))
	{
		if(++(pMe->blinkTicksCurrent) >= pMe->blinkTicksSet  )
		{
			TriColorLed_ToggleState(pMe);
			pMe->blinkTicksCurrent = 0 ;
		}
	}
}

/**
 * @brief Set TriColor LED state
 *
 * @param pMe TriColor LED instance
 * @param triColorLEDState
 */
static void TriColorLed_SetState(volatile sTricolorLED_t* const pMe, eTriColorLEDStates_t triColorLEDState)
{
	assert(NULL != pMe);
	assert(eLED_COLOR_STATE_MAX > triColorLEDState);

	pMe->triColorLEDState = triColorLEDState;

	if(triColorLEDState != eLED_COLOR_ALL_OFF)
	{
		pMe->prevOnLEDState = triColorLEDState;
	}

	sTriColorLEDState_t triColorLEDStateToSet = gcLEDStateHelperTable[triColorLEDState];

	for(int i=0; i<eLED_MAX; i++ )	/**< Set Primary set of LED*/
	{
		GPIO_PinState IndividualLedStateToSet = gcLEDEnumtoGPIOStateConverter[triColorLEDStateToSet.LedState[i]];

		pMe->Leds[i].ledState = IndividualLedStateToSet;
		HAL_GPIO_WritePin(pMe->Leds[i].pcLedPin->LEDPort, pMe->Leds[i].pcLedPin->LEDPin, IndividualLedStateToSet);
	}

	for(int i=0; i<eLED_MAX; i++ )	/**< Duplicate set of LED follows primary LED indication */
	{
		GPIO_PinState IndividualLedStateToSet = gcLEDEnumtoGPIOStateConverter[triColorLEDStateToSet.LedState[i]];

		pMe->Leds2[i].ledState = IndividualLedStateToSet;
		HAL_GPIO_WritePin(pMe->Leds2[i].pcLedPin->LEDPort, pMe->Leds2[i].pcLedPin->LEDPin, IndividualLedStateToSet);
	}
}

/**
 * @brief Toggle TriColor LED
 *
 * @param pMe TriColor LED instance
 */
static void TriColorLed_ToggleState(volatile sTricolorLED_t* const pMe)
{
	assert(NULL != pMe);

	pMe->triColorLEDState = (eLED_COLOR_ALL_OFF == pMe->triColorLEDState)? pMe->prevOnLEDState: eLED_COLOR_ALL_OFF;


	TriColorLed_SetState(pMe, pMe->triColorLEDState);
}

/**
 * @brief Initialize Tricolor LED
 *
 * @param pMe TriColor LED instance
 */
static void TriColorLed_Init(volatile sTricolorLED_t* const pMe)
{
	assert(NULL != pMe);

	pMe->triColorLEDState = eLED_COLOR_ALL_OFF;
	pMe->prevOnLEDState = eLED_COLOR_ALL_OFF;
	pMe->IsBlinkEnabled = false;
	pMe->blinkTicksSet = 0;
	pMe->blinkTicksCurrent = 0;
	pMe->IsInitialized = true;

	SoftTimer_Register(eDEBUG_LED_SOFT_TIMER, TRICOLOR_LED_BLINK_TIME_BASE_MS, true, TriColorLED_cbBlinkHandler  );
}

/**
 * @brief DeInitialize Tricolor LED
 *
 * @param pMe TriColor LED instance
 */
static void TriColorLed_DeInit(volatile sTricolorLED_t* const pMe)
{
	assert(NULL != pMe);

	pMe->triColorLEDState = eLED_COLOR_ALL_OFF;
	pMe->prevOnLEDState = eLED_COLOR_ALL_OFF;
	pMe->IsBlinkEnabled = false;
	pMe->blinkTicksSet = 0;
	pMe->blinkTicksCurrent = 0;
	pMe->IsInitialized = false;

}

/**
 * @brief Enable/Disable LED blink routine. The Blink will be executed though @ref TriColorLED_cbBlinkHandler
 *
 * @param pMe TriColor LED instance
 * @param eBlinkPeriod
 * @param IsStartNeeded Pass true if blink needed, false to cancel blink
 */
static void TriColorLed_StartBlink(volatile sTricolorLED_t* const pMe,  eTriColorLEDBlinkPeriod_t eBlinkPeriod, bool IsStartNeeded)
{
	assert(NULL != pMe);

	pMe->IsBlinkEnabled = IsStartNeeded;

	pMe->blinkTicksCurrent = 0;
	pMe->blinkTicksSet = gcBlinkPeriodToCountHelper[eBlinkPeriod];

	SoftTimer_Start(eDEBUG_LED_SOFT_TIMER, IsStartNeeded);
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize tricolor LED API
 *
 */
void TriColorLed_API_Init()
{
	volatile sTricolorLED_t* const pMe = TriColorLed_GetInstance();
	TriColorLed_Init(pMe);
}

/**
 * @brief DeInitialize tricolor LED API
 *
 */
void TriColorLed_API_DeInit()
{
	volatile sTricolorLED_t* const pMe = TriColorLed_GetInstance();
	TriColorLed_API_SetState(eLED_COLOR_ALL_OFF);
	TriColorLed_DeInit(pMe);
}

/**
 * @brief API to set Tricolor LED state
 *
 * @param state state to set TriColor LED
 */
void TriColorLed_API_SetState(eTriColorLEDStates_t state)
{
	volatile sTricolorLED_t* const pMe = TriColorLed_GetInstance();
	if(true == pMe->IsInitialized)
	{
		TriColorLed_SetState(pMe, state);
	}
}

/**
 * @brief Tricolor LED to indicate various Application states
 *
 * @param state TriColor LED color to set
 * @param eBlinkPeriod Period to blink LED over
 * @param IsBlinkNeeded Set to true if blinking is needed, else the LED will only light up
 */
void TriColorLed_API_Indicate(eTriColorLEDStates_t state, eTriColorLEDBlinkPeriod_t eBlinkPeriod, bool IsBlinkNeeded)
{
	volatile sTricolorLED_t* const pMe = TriColorLed_GetInstance();
	if(true == pMe->IsInitialized)
	{
		TriColorLed_SetState(pMe, state);
		TriColorLed_StartBlink(pMe, eBlinkPeriod, IsBlinkNeeded );
	}
}
