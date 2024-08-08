/**
 * @file DebugPrint.h
 * @author Vishal Keshava Murthy 
 * @brief Debug print definition is redirected to @ref Console_Print function if enabled 
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UTILITY_DEBUGPRINT_H_
#define UTILITY_DEBUGPRINT_H_

///////////////////////////////////////////////////////////////////////////////

#include "AppConfiguration.h"
#include "Console.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_DEBUG_PRINT
#define DEBUG_PRINT Console_Print /**< Debug print is redirected to Console_Print */
#else
#define DEBUG_PRINT(...)
#endif


#endif /* UTILITY_DEBUGPRINT_H_ */
