/**
 * @file AppInterrupts.h
 * @author Vishal Keshava Murthy
 * @brief Application Interrupt callbacks interface
 * @version 0.1
 * @date 2024-10-24
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPINTERRUPTS_APPINTERRUPTS_H_
#define APPINTERRUPTS_APPINTERRUPTS_H_

///////////////////////////////////////////////////////////////////////////////

#define LOOP_FOREVER                                                                                                                                           \
    {                                                                                                                                                          \
        while (1)                                                                                                                                              \
            ;                                                                                                                                                  \
    } ///< Utility MACRO to loop forever */

///////////////////////////////////////////////////////////////////////////////

void AppISR_HALErrorHandler();
void AppISR_ARMHandler();
void __assert_func(const char* file, int line, const char* func, const char* failedexpr);

///////////////////////////////////////////////////////////////////////////////

#endif /* APPINTERRUPTS_APPINTERRUPTS_H_ */
