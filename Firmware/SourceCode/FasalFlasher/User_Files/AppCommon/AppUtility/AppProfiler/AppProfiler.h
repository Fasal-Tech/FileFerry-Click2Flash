/**
 * @file AppProfiler.h
 * @author Vishal Keshava Murthy
 * @brief Utility Interface to measure function execution time
 * @version 0.1
 * @date 2024-06-13
 *
 * @copyright Copyright (c) 2023
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPCOMMON_APPUTILITY_APPPROFILER_APPPROFILER_H_
#define APPCOMMON_APPUTILITY_APPPROFILER_APPPROFILER_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////

void AppProfiler_StartExecutionTimeMeasurement();
uint32_t AppProfiler_GetExecutionTimeMS();

///////////////////////////////////////////////////////////////////////////////

#endif /* APPCOMMON_APPUTILITY_APPPROFILER_APPPROFILER_H_ */
