/*
 * time_measurement.c
 *
 *  Created on: Jun 9, 2021
 *      Author: florianfrank
 */

#include "time_measurement.h"

#define start_timer()    *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
#define stop_timer()   *((volatile uint32_t*)0xE0001000) = 0x40000000  // Disable CYCCNT register
#define get_timer()   *((volatile uint32_t*)0xE0001004)               // Get value from CYCCNT register
#define reset_timer() *((volatile uint32_t*)0xE0001004) = 0x00        // Reset counter

#define CLOCK_FREQUENCY		168 // MHz

inline uint32_t StartTimer()
{
	ResetTimer();
	start_timer();
	return get_timer();
}


inline void ResetTimer()
{
	reset_timer();
}

inline uint32_t StopGetTime()
{
 	uint32_t ret = get_timer();
	ResetTimer();
	return ret;
}

inline uint32_t TransformClockFrequencyToNs(uint32_t value)
{
	// Eine schwingung
	return (uint32_t)((float)value * (float)5.95238095);
}
