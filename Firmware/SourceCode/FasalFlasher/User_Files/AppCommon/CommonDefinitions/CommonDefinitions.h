/**
 * @file CommonDefinitions.h
 * @author Vishal Keshava Murthy
 * @brief Utility definitions and DS used in the code
 * @version 0.1
 * @date 2024-10-24
 *
 * @copyright Copyright (c) 2024
 *
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPCOMMON_COMMONDEFINITIONS_COMMONDEFINITIONS_H_
#define APPCOMMON_COMMONDEFINITIONS_COMMONDEFINITIONS_H_

///////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"

///////////////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE_4 (4u)
#define BUFFER_SIZE_8 (8u)
#define BUFFER_SIZE_16 (16u)
#define BUFFER_SIZE_32 (32u)
#define BUFFER_SIZE_64 (64u)
#define BUFFER_SIZE_128 (128u)
#define BUFFER_SIZE_256 (256u)
#define BUFFER_SIZE_512 (512u)
#define BUFFER_SIZE_1024 (1024u)

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Wrapper around STM GPIO pin structure
 *
 */
typedef struct
{
    GPIO_TypeDef* GpioPort;
    uint16_t GpioPin;
} sSTMGpioPinWrapper_t;

///////////////////////////////////////////////////////////////////////////////

#endif /* APPCOMMON_COMMONDEFINITIONS_COMMONDEFINITIONS_H_ */
