/**
 * @file TriColorLED.h
 * @author Vishal Keshava Murthy
 * @brief TriColor LED Interface 
 * @version 0.1
 * @date 2023-09-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPCOMMON_TRICOLORLED_TRICOLORLED_H_
#define APPCOMMON_TRICOLORLED_TRICOLORLED_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "gpio.h"

///////////////////////////////////////////////////////////////////////////////

#define TRICOLOR_LED_BLINK_TIME_BASE_MS	(50u) /** < This is the timebase from which all LED blink period is composed of */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief LEDs that make up tricolor LEDs
 * 
 */
typedef enum
{
	eLED_RED,
	eLED_GREEN,
	eLED_BLUE,
	eLED_MAX
}eLEDId_t;

/**
 * @brief LED state enumeration
 *
 */
typedef enum
{
	eLED_OFF,
	eLED_ON,
	eLED_STATE_MAX
}eLEDState_t;

/**
 * @brief Possible LED blink periods
 *
 */
typedef enum
{
	eTRICOLORLED_BLINK_NONE,
	eTRICOLORLED_BLINK_250MS,
	eTRICOLORLED_BLINK_500MS,
	eTRICOLORLED_BLINK_1000MS,
	eTRICOLORLED_BLINK_2000MS,
	eLED_BLINK_PERIOD_MAX
}eTriColorLEDBlinkPeriod_t;

/**
 * @brief TriColor LED state enumerations 
 * 
 */
typedef enum
{
	eLED_COLOR_ALL_OFF,
	eLED_COLOR_RED,
	eLED_COLOR_GREEN,
	eLED_COLOR_BLUE,
	eLED_COLOR_YELLOW,
	eLED_COLOR_MAGENTA,
	eLED_COLOR_CYAN,
	eLED_COLOR_WHITE,
	eLED_COLOR_STATE_MAX
}eTriColorLEDStates_t;

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Helper structure encompassing LED states
 * 
 */
typedef struct
{
	eLEDState_t LedState[eLED_MAX];
}sTriColorLEDState_t;

/**
 * @brief LED Pin Structure
 * 
 */
typedef struct
{
	GPIO_TypeDef* LEDPort;
	uint16_t LEDPin;
}sLEDPins_t;

/**
 * @brief LED structure
 * 
 */
typedef struct
{
	const sLEDPins_t* pcLedPin;
	eLEDState_t ledState;
}sLed_t;

/**
 * @brief TriColor LED structure
 * 
 */
typedef struct
{
	bool IsInitialized;
	bool IsBlinkEnabled;
	size_t blinkTicksSet;
	size_t blinkTicksCurrent;
	eTriColorLEDStates_t triColorLEDState;
	eTriColorLEDStates_t prevOnLEDState;
	sLed_t Leds[eLED_MAX];
	sLed_t Leds2[eLED_MAX];	/**< Duplicate set of LED pins that shows the same indication*/
}sTricolorLED_t;

/**
 * @brief Wrapper utility structure that combines @ref eTriColorLEDStates_t and @ref eTriColorLEDBlinkPeriod_t
 *
 */
typedef struct
{
	bool IsBlinkNeeded;
	eTriColorLEDStates_t ledState;
	eTriColorLEDBlinkPeriod_t ledBlinkPeriod;
}sTriColorIndication_t;

///////////////////////////////////////////////////////////////////////////////

void TriColorLed_API_Init();
void TriColorLed_API_DeInit();
void TriColorLed_API_SetState(eTriColorLEDStates_t state);
void TriColorLed_API_Indicate(eTriColorLEDStates_t state, eTriColorLEDBlinkPeriod_t eBlinkPeriod, bool IsBlinkNeeded);

///////////////////////////////////////////////////////////////////////////////

#endif /* APPCOMMON_TRICOLORLED_TRICOLORLED_H_ */
