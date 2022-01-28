/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYMODULE_H
#define MEMORY_TESTING_FW_MEMORYMODULE_H
#include "cpp/MemoryModules/MemoryProperties.h"
#include "cpp/InterfaceWrappers/InterfaceWrapper.h"
#include "cpp/MemoryErrorHandling.h"

#include <string> // std::string

class MemoryController;

using namespace MemoryProperties;
using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

/**
 * @brief This abstract class provides all the required functions to be implemented by a new memory module.
 */
class MemoryModule
{
public:
    MemoryModule() = delete;
    virtual MEM_ERROR Initialize();

    virtual MEM_ERROR CreateWriteMessage(uint32_t address, uint8_t valueToWrite, uint8_t *returnSendBuffer, uint16_t *sendBufferSize);
    virtual MEM_ERROR CreateReadMessage(uint32_t address, uint8_t *sendBuffer, uint16_t *sendBufferSize);

protected:
    MemoryModule(Type memoryType, const char *memoryLabel, Connection connectionType, uint8_t bitWidth,
                 uint32_t memorySize, TemperatureProperties temperatureProperties, VoltageProperties voltageProperties);
    virtual ~MemoryModule();

    MEM_ERROR Initialize(InterfaceWrapper *interfaceWrapper);

public:
    Type GetMemoryType() const;

    const std::string &GetMemoryLabel() const;

    Connection GetConnectionType() const;

    uint8_t GetBitWidth() const;

    uint32_t GetMemorySize() const;

    const TemperatureProperties &GetTemperatureProperties() const;

    const VoltageProperties &GetVoltageProperties() const;

protected:
    Type m_MemoryType;
    std::string m_MemoryLabel;
    Connection m_ConnectionType;
    uint8_t m_BitWidth;
    uint32_t m_MemorySize;
    TemperatureProperties m_TemperatureProperties;
    VoltageProperties m_VoltageProperties;
};
#endif //MEMORY_TESTING_FW_MEMORYMODULE_H