/**
 * @file AppIndication.c
 * @author Vishal Keshava Murthy 
 * @brief This module abstracts Indication medium on board and map to application state 
 * @version 0.1
 * @date 2024-05-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "AppIndication.h"
#include "TriColorLED.h"

///////////////////////////////////////////////////////////////////////////////
/**
 * @brief Configuration table that determine how application states correspond to TriColor LED states 
 * 
 */
static sTriColorIndication_t gcAppIndicationTable[eIND_MAX] =
{
		[eIND_NO_CHANGE] 		= {.IsBlinkNeeded = false, 	.ledState = eLED_COLOR_ALL_OFF, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_NONE] 			= {.IsBlinkNeeded = false, 	.ledState = eLED_COLOR_ALL_OFF, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},

		[eIND_RED_0] 			= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_RED, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_GREEN_0] 			= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_GREEN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_BLUE_0] 			= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_BLUE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_YELLOW_0] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_YELLOW, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_MAGENTA_0] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_MAGENTA, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_CYAN_0] 			= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_CYAN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},
		[eIND_WHITE_0] 			= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_WHITE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_NONE},

		[eIND_RED_250MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_RED, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},
		[eIND_GREEN_250MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_GREEN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},
		[eIND_BLUE_250MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_BLUE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},
		[eIND_YELLOW_250MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_YELLOW, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},
		[eIND_MAGENTA_250MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_MAGENTA, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},
		[eIND_CYAN_250MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_CYAN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},
		[eIND_WHITE_250MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_WHITE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_250MS},

		[eIND_RED_500MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_RED, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},
		[eIND_GREEN_500MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_GREEN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},
		[eIND_BLUE_500MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_BLUE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},
		[eIND_YELLOW_500MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_YELLOW, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},
		[eIND_MAGENTA_500MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_MAGENTA, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},
		[eIND_CYAN_500MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_CYAN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},
		[eIND_WHITE_500MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_WHITE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_500MS},

		[eIND_RED_1000MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_RED, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},
		[eIND_GREEN_1000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_GREEN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},
		[eIND_BLUE_1000MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_BLUE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},
		[eIND_YELLOW_1000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_YELLOW, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},
		[eIND_MAGENTA_1000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_MAGENTA, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},
		[eIND_CYAN_1000MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_CYAN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},
		[eIND_WHITE_1000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_WHITE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_1000MS},

		[eIND_RED_2000MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_RED, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},
		[eIND_GREEN_2000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_GREEN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},
		[eIND_BLUE_2000MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_BLUE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},
		[eIND_YELLOW_2000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_YELLOW, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},
		[eIND_MAGENTA_2000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_MAGENTA, 	.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},
		[eIND_CYAN_2000MS] 		= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_CYAN, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},
		[eIND_WHITE_2000MS] 	= {.IsBlinkNeeded = true, 	.ledState = eLED_COLOR_WHITE, 		.ledBlinkPeriod = eTRICOLORLED_BLINK_2000MS},

};

///////////////////////////////////////////////////////////////////////////////

static eAppIndicationStates_t gCurrentState = eIND_NONE;	/**< Current Indication state maintained in this global */
static eAppIndicationStates_t gPrevState = eIND_NONE;		/**< Previous Indication state maintained here*/

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Wrapper around Indication medium initialization
 * 
 */
void AppIndicate_Init()
{
	TriColorLed_API_Init();
}

/**
 * @brief Wrapper around Indication medium De-initialization
 *
 */
void AppIndicate_DeInit()
{
	TriColorLed_API_DeInit();
}

/**
 * @brief Set Indication medium state 
 * 
 * @param state Which application state to set to, @ref gcAppIndicationTable
 */
void AppIndicate_SetState(eAppIndicationStates_t state)
{
	assert(state < eIND_MAX);

	if( (state != gCurrentState) && (eIND_NO_CHANGE != state))
	{
		gPrevState = gCurrentState;
		gCurrentState = state;
		TriColorLed_API_Indicate(gcAppIndicationTable[state].ledState, gcAppIndicationTable[state].ledBlinkPeriod, gcAppIndicationTable[state].IsBlinkNeeded );
	}
}

/**
 * @brief Revert to previous indication state. For preserving previous Indication state 
 * 
 */
void AppIndicate_RevertState()
{
	TriColorLed_API_Indicate(gcAppIndicationTable[gPrevState].ledState, gcAppIndicationTable[gPrevState].ledBlinkPeriod, gcAppIndicationTable[gPrevState].IsBlinkNeeded );
}

///////////////////////////////////////////////////////////////////////////////
