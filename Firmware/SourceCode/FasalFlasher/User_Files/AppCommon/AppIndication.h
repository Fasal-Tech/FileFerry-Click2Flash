/**
 * @file AppIndication.h
 * @author Vishal Keshava Murthy 
 * @brief Interface for Indicating various application states over LEDs
 * @version 0.1
 * @date 2024-05-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPCOMMON_APPINDICATION_H_
#define APPCOMMON_APPINDICATION_H_

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Application events that require LED indication are enumerated here @ref gcAppIndicationTable
 * 
 */
typedef enum
{
	eIND_NO_CHANGE,
	eIND_NONE,

	eIND_RED_0,
	eIND_GREEN_0,
	eIND_BLUE_0,
	eIND_YELLOW_0,
	eIND_MAGENTA_0,
	eIND_CYAN_0,
	eIND_WHITE_0,

	eIND_RED_250MS,
	eIND_GREEN_250MS,
	eIND_BLUE_250MS,
	eIND_YELLOW_250MS,
	eIND_MAGENTA_250MS,
	eIND_CYAN_250MS,
	eIND_WHITE_250MS,

	eIND_RED_500MS,
	eIND_GREEN_500MS,
	eIND_BLUE_500MS,
	eIND_YELLOW_500MS,
	eIND_MAGENTA_500MS,
	eIND_CYAN_500MS,
	eIND_WHITE_500MS,

	eIND_RED_1000MS,
	eIND_GREEN_1000MS,
	eIND_BLUE_1000MS,
	eIND_YELLOW_1000MS,
	eIND_MAGENTA_1000MS,
	eIND_CYAN_1000MS,
	eIND_WHITE_1000MS,

	eIND_RED_2000MS,
	eIND_GREEN_2000MS,
	eIND_BLUE_2000MS,
	eIND_YELLOW_2000MS,
	eIND_MAGENTA_2000MS,
	eIND_CYAN_2000MS,
	eIND_WHITE_2000MS,

	eIND_MAX,
}eAppIndicationStates_t;

///////////////////////////////////////////////////////////////////////////////

void AppIndicate_Init();
void AppIndicate_SetState(eAppIndicationStates_t state);
void AppIndicate_RevertState();

///////////////////////////////////////////////////////////////////////////////

#endif /* APPCOMMON_APPINDICATION_H_ */
