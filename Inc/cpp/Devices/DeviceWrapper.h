//
// Created by florianfrank on 14.12.21.
//

#ifndef MEMORY_TESTING_FW_DEVICEWRAPPER_H
#define MEMORY_TESTING_FW_DEVICEWRAPPER_H

#include "cpp/MemoryErrorHandling.h"

class DeviceWrapper
{
    virtual MemoryErrorHandling::MEM_ERROR InitializeDevice() = 0;
    virtual MemoryErrorHandling::MEM_ERROR DeInitializeDevice() = 0;
};


#endif //MEMORY_TESTING_FW_DEVICEWRAPPER_H
