/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
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
{}


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
        m_MemoryController = new MemoryControllerSPI(reinterpret_cast<SPIWrapper *>(interfaceWrapper), <#initializer#>); // TODO
        return MemoryErrorHandling::MEM_NO_ERROR;
    }
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}

Type MemoryModule::GetMemoryType() const
{
    return m_MemoryType;
}

const std::string &MemoryModule::GetMemoryLabel() const
{
    return m_MemoryLabel;
}

Connection MemoryModule::GetConnectionType() const
{
    return m_ConnectionType;
}

uint8_t MemoryModule::GetBitWidth() const
{
    return m_BitWidth;
}

uint32_t MemoryModule::GetMemorySize() const
{
    return m_MemorySize;
}

const TemperatureProperties &MemoryModule::GetTemperatureProperties() const
{
    return m_TemperatureProperties;
}

const VoltageProperties &MemoryModule::GetVoltageProperties() const
{
    return m_VoltageProperties;
}

MemoryController *MemoryModule::GetMemoryController() const
{
    return m_MemoryController;
}
