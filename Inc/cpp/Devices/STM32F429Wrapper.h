//
// Created by florianfrank on 14.12.21.
//

#ifndef MEMORY_TESTING_FW_STM32F429WRAPPER_H
#define MEMORY_TESTING_FW_STM32F429WRAPPER_H


#include <cpp/MemoryErrorHandling.h>
#include "DeviceWrapper.h"

class STM32F429Wrapper: public DeviceWrapper
{
public:
    MemoryErrorHandling::MEM_ERROR InitializeDevice() override;

    MemoryErrorHandling::MEM_ERROR DeInitializeDevice() override;

private:
    static MemoryErrorHandling::MEM_ERROR SystemClock_Config();
};


#endif //MEMORY_TESTING_FW_STM32F429WRAPPER_H
