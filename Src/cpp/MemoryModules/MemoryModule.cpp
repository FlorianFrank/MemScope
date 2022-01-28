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
                                                                    m_VoltageProperties(voltageProperties)
{}


MemoryModule::~MemoryModule()
{
}

MEM_ERROR MemoryModule::Initialize(InterfaceWrapper *interfaceWrapper)
{
    return MemoryErrorHandling::MEM_NO_ERROR;
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


MEM_ERROR MemoryModule::Initialize()
{
    return MemoryErrorHandling::MEM_MEMORY_NOT_SUPPORTED;
}

MEM_ERROR MemoryModule::CreateWriteMessage(uint32_t address, uint8_t valueToWrite, uint8_t *returnSendBuffer,
                                           uint16_t *sendBufferSize)
{
    return MemoryErrorHandling::MEM_MEMORY_NOT_SUPPORTED; // TODO parallele speicher
}

MEM_ERROR MemoryModule::CreateReadMessage(uint32_t address, uint8_t *sendBuffer, uint16_t *sendBufferSize)
{
    return MemoryErrorHandling::MEM_MEMORY_NOT_SUPPORTED; // TODO parallele speicher
}
