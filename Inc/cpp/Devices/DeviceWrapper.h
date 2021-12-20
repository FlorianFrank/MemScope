/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_DEVICEWRAPPER_H
#define MEMORY_TESTING_FW_DEVICEWRAPPER_H

#include "cpp/MemoryErrorHandling.h"

class DeviceWrapper
{
    virtual MemoryErrorHandling::MEM_ERROR Initialize() = 0;
    virtual MemoryErrorHandling::MEM_ERROR DeInitialize() = 0;
};


#endif //MEMORY_TESTING_FW_DEVICEWRAPPER_H
