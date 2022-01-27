/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_TIMEMEASUREMENT_H
#define MEMORY_TESTING_FW_TIMEMEASUREMENT_H

#include <cstdint> // uint32_t

/**
 * @brief This class provides functions to measure the elapsed time using Cortex M4 registers or
 * std::chrono::system_clock on linux systems.
 */
class TimeMeasurement
{
public:

    void StarTimer();
    uint32_t StopTimer();
    uint32_t GetTimer() const;
    void ResetTimer();
    uint32_t ResetAndStartTimer();

    uint32_t GetRawCounter() const;
    uint32_t GetElapsedTimeInNS() const;
    uint32_t GetElapsedTimeInMS() const;

    static uint32_t TransformClockFrequencyToNs(uint32_t value);
    static uint32_t TransformClockFrequencyToMs(uint32_t value);

private:
    unsigned long long m_StartTimer;
    unsigned long long m_StopTimer;
};
#endif //MEMORY_TESTING_FW_TIMEMEASUREMENT_H
