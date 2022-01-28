//
// Created by florianfrank on 14.12.21.
//

#include "cpp/Devices/DeviceWrapper.h"

DeviceWrapper::DeviceWrapper()
{

}

/*virtual*/ MemoryErrorHandling::MEM_ERROR DeviceWrapper::Initialize(){
    return MemoryErrorHandling::MEM_DEVICE_NOT_SPECIFIED;
}

/*virtual*/ MemoryErrorHandling::MEM_ERROR DeviceWrapper::DeInitialize()
{
    return MemoryErrorHandling::MEM_DEVICE_NOT_SPECIFIED;
}

/*virtual*/ MemoryErrorHandling::MEM_ERROR DeviceWrapper::InitializeHardwareInterface(std::string &interfaceName)
{
    return MemoryErrorHandling::MEM_DEVICE_NOT_SPECIFIED;
}
