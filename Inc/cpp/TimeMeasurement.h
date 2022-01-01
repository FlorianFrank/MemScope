/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_TIMEMEASUREMENT_H
#define MEMORY_TESTING_FW_TIMEMEASUREMENT_H

#include <cstdint>

/**
 * @brief This class provides functions to measure the elapsed time using Cortex M4 registers
 */
class TimeMeasurement{
public:
    static uint32_t ResetAndStartTimer();
    static uint32_t GetClockCycles();
    static uint32_t GetElapsedTimeInNS();
    static void ResetTimer();
    static void StopTimer();
    static uint32_t GetTimer();
    static uint32_t TransformClockFrequencyToNs(uint32_t value);
};


#endif //MEMORY_TESTING_FW_TIMEMEASUREMENT_H
