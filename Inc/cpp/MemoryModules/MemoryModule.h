//
// Created by florianfrank on 23.12.21.
//

#ifndef MEMORY_TESTING_FW_MEMORYMODULE_H
#define MEMORY_TESTING_FW_MEMORYMODULE_H

#include "cpp/MemoryModules/MemoryProperties.h"
#include "cpp/MemoryControllers/MemoryController.h"

#include <string>
#include <cstdint>

using namespace MemoryProperties;
using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class MemoryModule
{
public:
    MemoryModule() = delete;

protected:
    MemoryModule(Type memoryType, const char *memoryLabel, Connection connectionType, uint8_t bitWidth,
                 uint32_t memorySize, TemperatureProperties temperatureProperties, VoltageProperties voltageProperties);

    virtual ~MemoryModule();

    MEM_ERROR Initialize(InterfaceWrapper *interfaceWrapper);

private:
    Type m_MemoryType;
    std::string m_MemoryLabel;
    Connection m_ConnectionType;
    uint8_t m_BitWidth;
    uint32_t m_MemorySize;
    TemperatureProperties m_TemperatureProperties;
    VoltageProperties m_VoltageProperties;

    MemoryController *m_MemoryController;
};

#endif //MEMORY_TESTING_FW_MEMORYMODULE_H