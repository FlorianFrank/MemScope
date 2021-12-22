/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_DEVICEWRAPPER_H
#define MEMORY_TESTING_FW_DEVICEWRAPPER_H

#include "cpp/MemoryErrorHandling.h"

#include <string>

class DeviceWrapper
{
public:
    virtual ~DeviceWrapper()= default;
    virtual MemoryErrorHandling::MEM_ERROR Initialize();
    virtual MemoryErrorHandling::MEM_ERROR DeInitialize();
    virtual MemoryErrorHandling::MEM_ERROR InitializeInterface(std::string& interfaceName);
};


#endif //MEMORY_TESTING_FW_DEVICEWRAPPER_H
