//
// Created by Florian Frank on 19.03.25.
//
#include "cpp/Experiments/WriteLatency.h"
#include "Logger.h"
#include "cpp/MemoryControllers/MemoryController.h"


WriteLatency::WriteLatency(MemoryController &memoryController, PUFConfiguration &pufConfig) : MemoryExperiment(
        memoryController, pufConfig) {
}

MemoryErrorHandling::MEM_ERROR WriteLatency::init() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Write Latency Experiment -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory();
}

MemoryErrorHandling::MEM_ERROR WriteLatency::running() {

    // TODO adjust timing
    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto initValue = m_PUFConfiguration.generalConfig.initValue;
    auto pufValue = m_PUFConfiguration.generalConfig.pufValue;

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Write value %0x00 with reduced timing in range [0x%x,0x%x] ", initValue, startAddress, endAddress);

    for (auto address = startAddress; address < endAddress; address++) {
        if (m_MemoryController.getMemoryModule().GetBitWidth() == 8) {
            uint8_t readValue;
            m_MemoryController.Write8BitWord(address, pufValue);
            // TODO FORWARD TO INTERFACE
        } else if (m_MemoryController.getMemoryModule().GetBitWidth() == 16) {
            uint16_t readValue;
            m_MemoryController.Write16BitWord(address, pufValue);
            // TODO FORWARD TO INTERFACE
        } else {
            Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Bit width %d not supported",
                        m_MemoryController.getMemoryModule().GetBitWidth());
            return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
        }
    }

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Write Latency done");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR WriteLatency::done() {

    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto initValue = m_PUFConfiguration.generalConfig.initValue;

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Verify written results in range [0x%x,0x%x] ",
                startAddress, endAddress, initValue);

    for (auto addr = startAddress; addr < endAddress; addr++) {
        if (m_MemoryController.getMemoryModule().GetBitWidth() == 8) {
            uint8_t readValue;
            m_MemoryController.Read8BitWord(addr, &readValue);
            // TODO FORWARD TO INTERFACE
        } else if (m_MemoryController.getMemoryModule().GetBitWidth() == 16) {
            uint16_t readValue;
            m_MemoryController.Read16BitWord(addr, &readValue);
            // TODO FORWARD TO INTERFACE
        } else {
            Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Bit width %d not supported",
                        m_MemoryController.getMemoryModule().GetBitWidth());
            return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
        }
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}
