/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/TimeMeasurement.h"


#if STM32
// Clock frequency is used to convert the clock counts into nanoseconds
#define MULTIPLIER		1/168 * 1e9 // TODO set dynamically
#else
#include <chrono>
#define MULTIPLIER		1
#endif // STM32

/**
 * @brief Resets the previous timer by disabling the previous counters
 * and restarts the measurements by setting the CYCCNT register on STM32
 * or initializing m_StartTimer by ns since 01.01.1970 using chrono::system_clock()
 * @return start timer value.
 */
uint32_t TimeMeasurement::ResetAndStartTimer()
{
    ResetTimer();
    StarTimer();
    return m_StartTimer;
}

/**
 * @brief This function sets the startTimer member variable with a current counter value.
 * When using linux systems, the attribute is initialized with the ns since 01.01.1970 using chrono::system_clock().
 * On Stm32 Systems it is initialized with the current clock count.
 */
void TimeMeasurement::StarTimer()
{
#if STM32
    // resolution in clock cycles
    *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
    m_StartTimer = *((volatile uint32_t*)0xE0001004);
#else
    // Resolution in nanoseconds
    m_StartTimer = std::chrono::system_clock::now().time_since_epoch().count();
#endif
}

/**
 * @brief This method stops the timer and stores the timer value into the m_StopTimer variable.
 * When using linux systems, the attribute is initialized with the ns since 01.01.1970 using chrono::system_clock().
 * @return stop timer.
 */
uint32_t TimeMeasurement::StopTimer()
{
#if STM32
    *((volatile uint32_t*)0xE0001000) = 0x40000000;  // Disable CYCCNT register
    m_StopTimer = *((volatile uint32_t*)0xE0001004);
#else
    m_StopTimer = std::chrono::system_clock::now().time_since_epoch().count();
#endif
    return m_StopTimer;
}

/**
 * @brief Returns the current timer value, if the measurement was not stoped.
 * If the measurement was stopped previously, the difference between the start and stop timer is returned.
 * @return Timer value.
 */
uint32_t TimeMeasurement::GetTimer() const
{
    if(m_StopTimer != 0)
        return m_StopTimer - m_StartTimer;
    else
#if STM32
        return *((volatile uint32_t*)0xE0001004); - m_StartTimer;
#else
        return std::chrono::system_clock::now().time_since_epoch().count() - m_StartTimer;
#endif
}

/**
 * @brief Resets the counter used to count the clock cycles. Sets start and stop timer to 0.
 */
void TimeMeasurement::ResetTimer()
{
#if STM32
    *((volatile uint32_t*)0xE0001004) = 0x00;        // Reset counter
#endif // STM32
    m_StartTimer = 0;
    m_StartTimer = 0;
}

/**
 * @brief Return the raw timer value of a counter (difference between start and stop timer).
 * On STM32 boards, the amount of clock cycles is returned on linux systems, the difference in ns is returned.
 */
uint32_t TimeMeasurement::GetRawCounter() const
{
    return m_StopTimer - m_StartTimer;
}

/**
 * @brief Returns the elapsed time since calling StartTimer() in nanosecond resolution.
 * @return elapsed time in ns.
 */
uint32_t TimeMeasurement::GetElapsedTimeInNS() const
{
    return TransformClockFrequencyToNs(GetTimer());
}

/**
 * @brief Returns the elapsed time since calling StartTimer() in milliseconds resolution.
 * @return elapsed time in ms.
 */
uint32_t TimeMeasurement::GetElapsedTimeInMS() const
{
    return TransformClockFrequencyToMs(GetTimer());
}

/**
 * @brief Transforms the clock counts based on the frequency to nano seconds.
 * @param value clock count value.
 * @return elapsed time in ns.
 */
/*static*/ uint32_t TimeMeasurement::TransformClockFrequencyToNs(uint32_t value)
{
        return static_cast<uint32_t>((float)value * MULTIPLIER);
}

/*static*/ uint32_t TimeMeasurement::TransformClockFrequencyToMs(uint32_t value)
{
        return static_cast<uint32_t>(((float)value * MULTIPLIER) / 1e6);
}







