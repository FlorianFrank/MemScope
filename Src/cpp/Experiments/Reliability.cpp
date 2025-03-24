/**
 * @file Reliability.cpp
 * @brief Implementation of the Reliability experiment.
 *
 * This experiment verifies the reliability of memory operations by initializing the memory
 * and verifying written results after execution.
 *
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */

#include "cpp/Experiments/Reliability.h"
#include "Logger.h"
#include "cpp/MemoryControllers/MemoryController.h"

Reliability::Reliability(MemoryController &memoryController, PUFConfiguration &pufConfig,
                         InterfaceWrapper &interfaceWrapper) : MemoryExperiment(
        memoryController, pufConfig, interfaceWrapper) {
}

/**
 * @brief Initializes the Reliability experiment.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR Reliability::init() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Reliability Experiment -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory();
}

/**
 * @brief Runs the Reliability experiment.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR Reliability::running() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Do not perform any action, as memory initialized in init function");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Finalizes the Reliability experiment and verifies written results.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR Reliability::done() {
    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto initValue = m_PUFConfiguration.generalConfig.initValue;

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Verify written results in range [0x%x,0x%x] ",
                startAddress, endAddress, initValue);

    char sendDataBuf[128];
    auto sendData = [&sendDataBuf](InterfaceWrapper &interfaceWrapper, uint16_t addr, uint16_t returnValue) {
        int actualSize = snprintf(sendDataBuf, 128, "m:%x,%x,%x\n", addr, returnValue, addr + returnValue);
        if (actualSize > 0) {
            uint16_t size = actualSize;
            interfaceWrapper.SendData(reinterpret_cast<uint8_t *>(sendDataBuf), &size, 1000, false);
        } else {
            Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "snprintf returned error %s", strerror(errno));
        }
    };

    for (auto addr = startAddress; addr < endAddress + 1; addr++) {
        if (m_MemoryController.getMemoryModule().GetBitWidth() == 8) {
            uint8_t readValue;
            m_MemoryController.Read8BitWord(addr, &readValue);
            if (readValue != initValue)
                sendData(m_InterfaceWrapper, addr, readValue);
        } else if (m_MemoryController.getMemoryModule().GetBitWidth() == 16) {
            uint16_t readValue;
            m_MemoryController.Read16BitWord(addr, &readValue);
            if (readValue != initValue)
                sendData(m_InterfaceWrapper, addr, readValue);
        } else {
            Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Bit width %d not supported",
                        m_MemoryController.getMemoryModule().GetBitWidth());
            return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
        }
    }
    sendDataBuf[0] = '\0';
    uint16_t size = 1;
    m_InterfaceWrapper.SendData(reinterpret_cast<uint8_t *>(sendDataBuf), &size, 1000, true);
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Reading done");
    return MemoryErrorHandling::MEM_NO_ERROR;
}
