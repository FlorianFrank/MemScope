//
// Created by florianfrank on 05.12.21.
//

#include "../../Inc/cpp/TimeMeasurement.h"

/*
 * time_measurement.c
 *
 *  Created on: Jun 9, 2021
 *      Author: florianfrank
 */

#include "cpp/TimeMeasurement.h"

#define start_timer()    *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
#define stop_timer()   *((volatile uint32_t*)0xE0001000) = 0x40000000  // Disable CYCCNT register
#define get_timer()   *((volatile uint32_t*)0xE0001004)               // Get value from CYCCNT register
#define reset_timer() *((volatile uint32_t*)0xE0001004) = 0x00        // Reset counter

#define CLOCK_FREQUENCY		168 // MHz

inline uint32_t TimeMeasurement::StartTimer()
{
    ResetTimer();
    start_timer();
    return get_timer();
}

uint32_t TimeMeasurement::GetTimer()
{
    uint32_t ret = get_timer();
}

inline void TimeMeasurement::ResetTimer()
{
    reset_timer();
}

inline uint32_t TimeMeasurement::StopGetTime()
{
    uint32_t ret = GetTimer();
    ResetTimer();
    return ret;
}

inline uint32_t TimeMeasurement::TransformClockFrequencyToNs(uint32_t value)
{
    // Eine schwingung
    return (uint32_t)((float)value * (float)5.95238095);
}


