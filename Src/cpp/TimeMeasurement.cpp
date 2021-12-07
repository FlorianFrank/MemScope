/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/TimeMeasurement.h"

#define start_timer()    *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
#define stop_timer()     *((volatile uint32_t*)0xE0001000) = 0x40000000  // Disable CYCCNT register
#define get_timer()      *((volatile uint32_t*)0xE0001004)               // Get value from CYCCNT register
#define reset_timer()    *((volatile uint32_t*)0xE0001004) = 0x00        // Reset counter

// Clock frequency is used to convert the clock counts into nanoseconds
#define CLOCK_FREQUENCY		168 // TODO set dynamically

/**
 * @brief Resets the previous timer by resetting the counter
 * and restarts the measurements by setting the CYCCNT register
 * @return start timer value TODO is this really necessary?
 */
uint32_t TimeMeasurement::ResetAndStartTimer()
{
    ResetTimer();
    start_timer();
    return get_timer();
}


void TimeMeasurement::StopTimer()
{
    stop_timer();
}

/**
 * @brief Returns the current timer value.
 * @return Timer value.
 */
/*static*/ uint32_t TimeMeasurement::GetTimer()
{
    return get_timer();
}

/**
 * @brief Resets the counter used to count the clock cycles.
 */
/*static*/ void TimeMeasurement::ResetTimer()
{
    reset_timer();
}

/**
 * @brief Resets the counter used to count the clock cycles.
 */
/*static*/ uint32_t TimeMeasurement::GetClockCycles()
{
    uint32_t ret = GetTimer();
    ResetTimer();
    return ret;
}

/**
 * @brief Returns the elapsed time since calling StartTimer() in nanoseconds.
 * @return elapsed time in ns.
 */
uint32_t TimeMeasurement::GetElapsedTimeInNS()
{
    return TransformClockFrequencyToNs(GetTimer());
}

/**
 * @brief Transforms the clock counts based on the frequency to nano seconds.
 * @param value clock count value.
 * @return elapsed time in ns.
 */
/*static*/ uint32_t TimeMeasurement::TransformClockFrequencyToNs(uint32_t value)
{
    return static_cast<uint32_t>((float)value * 1/CLOCK_FREQUENCY);
}




