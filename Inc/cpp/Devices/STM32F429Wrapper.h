/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_STM32F429WRAPPER_H
#define MEMORY_TESTING_FW_STM32F429WRAPPER_H


#include <cpp/MemoryErrorHandling.h>
#include "DeviceWrapper.h"

/**
 * @brief This class wraps functions used by the STM32F429 board.
 * Like initializing the system clock or the hardware abstraction layers.
 */
class STM32F429Wrapper: public DeviceWrapper
{
public:
    MemoryErrorHandling::MEM_ERROR Initialize() override;
    MemoryErrorHandling::MEM_ERROR DeInitialize() override;

private:
    static MemoryErrorHandling::MEM_ERROR SystemClock_Config();
};


#endif //MEMORY_TESTING_FW_STM32F429WRAPPER_H
