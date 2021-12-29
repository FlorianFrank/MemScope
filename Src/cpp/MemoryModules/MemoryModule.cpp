//
// Created by florianfrank on 23.12.21.
//

#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include <cpp/MemoryControllers/MemoryControllerSPI.h>
#include "cpp/MemoryModules/MemoryModule.h"

MemoryModule::MemoryModule(Type memoryType, const char *memoryLabel, Connection connectionType,
                           uint8_t bitWidth, uint32_t memorySize, TemperatureProperties temperatureProperties,
                           VoltageProperties voltageProperties) : m_MemoryType(memoryType),
                                                                    m_MemoryLabel(memoryLabel),
                                                                    m_ConnectionType(connectionType),
                                                                    m_BitWidth(bitWidth), m_MemorySize(memorySize),
                                                                    m_TemperatureProperties(temperatureProperties),
                                                                    m_VoltageProperties(voltageProperties),
                                                                    m_MemoryController(nullptr)
{
}


MemoryModule::~MemoryModule()
{
    delete m_MemoryController;
}

MEM_ERROR MemoryModule::Initialize(InterfaceWrapper *interfaceWrapper)
{
    if(m_ConnectionType == MemoryProperties::PARALLEL)
    {
        m_MemoryController = new MemoryControllerParallel(interfaceWrapper);
        return MemoryErrorHandling::MEM_NO_ERROR;
    }
    else if(m_ConnectionType == MemoryProperties::SPI)
    {
        m_MemoryController = new MemoryControllerSPI(reinterpret_cast<SPIWrapper *>(interfaceWrapper)); // TODO
        return MemoryErrorHandling::MEM_NO_ERROR;
    }
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}
