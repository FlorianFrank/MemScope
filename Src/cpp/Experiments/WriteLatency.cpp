/**
 * @file WriteLatency.cpp
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 * @brief Implementation of Write Latency experiment for PUF-based memory analysis.
 */
#include "cpp/Experiments/WriteLatency.h"
#include "Logger.h"
#include "cpp/MemoryControllers/MemoryController.h"

/**
 * @brief Constructor for WriteLatency experiment.
 * @param memoryController Reference to the MemoryController.
 * @param pufConfig Reference to the PUFConfiguration.
 * @param interfaceWrapper Reference to the InterfaceWrapper.
 */
WriteLatency::WriteLatency(MemoryController &memoryController, PUFConfiguration &pufConfig,
                           InterfaceWrapper &interfaceWrapper) : MemoryExperiment(
        memoryController, pufConfig, interfaceWrapper) {
}

/**
 * @brief Initializes memory under normal timing.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR WriteLatency::init() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Write Latency Experiment -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory();
}

/**
 * @brief Writes values while intentionally violating the timing protocol.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR WriteLatency::running() {
    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto initValue = m_PUFConfiguration.generalConfig.initValue;
    auto pufValue = m_PUFConfiguration.generalConfig.pufValue;

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Set the memories default timing parameters");
    std::map<std::string, uint16_t> timingMap = {
            {"addressSetupTime",      m_PUFConfiguration.writeTimingConfigAdjusted.tAS},
            {"addressHoldTime",       m_PUFConfiguration.writeTimingConfigAdjusted.tAH},
            {"dataSetupTime",         m_PUFConfiguration.writeTimingConfigAdjusted.tPWD},
            {"busTurnAroundDuration", 4},
            {"clkDivision",           8},
            {"dataLatency",           0}
    };
    m_MemoryController.SetTimingParameters(timingMap);

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Write value %0x00 with reduced timing in range [0x%x,0x%x] ", initValue, startAddress, endAddress);

    char sendDataBuf[128];
    auto sendData = [&sendDataBuf](InterfaceWrapper &interfaceWrapper, uint16_t addr, uint16_t returnValue) {
        int actualSize = snprintf(sendDataBuf, 128, "0x%x, 0x%0x, 0x%0x\n", addr, returnValue, addr + returnValue);
        if (actualSize > 0) {
            uint16_t size = actualSize;
            interfaceWrapper.SendData(reinterpret_cast<uint8_t *>(sendDataBuf), &size, 1000, false);
        } else {
            Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "snprintf returned error %s", strerror(errno));
        }
    };

    for (auto address = startAddress; address < endAddress; address++) {
        if (m_MemoryController.getMemoryModule().GetBitWidth() == 8) {
            uint8_t readValue;
            m_MemoryController.Write8BitWord(address, pufValue);
            sendData(m_InterfaceWrapper, address, pufValue);
        } else if (m_MemoryController.getMemoryModule().GetBitWidth() == 16) {
            uint16_t readValue;
            m_MemoryController.Write16BitWord(address, pufValue);
            sendData(m_InterfaceWrapper, address, pufValue);
        } else {
            Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Bit width %d not supported",
                        m_MemoryController.getMemoryModule().GetBitWidth());
            return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
        }
    }

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Write Latency done");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Reads values under normal timing to verify the written results.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR WriteLatency::done() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Set the memories default timing parameters");
    std::map<std::string, uint16_t> timingMap = {
            {"addressSetupTime",      m_PUFConfiguration.readTimingConfigDefault.tAS},
            {"addressHoldTime",       m_PUFConfiguration.readTimingConfigDefault.tAH},
            {"dataSetupTime",         m_PUFConfiguration.readTimingConfigDefault.tPRC},
            {"busTurnAroundDuration", 4},
            {"clkDivision",           8},
            {"dataLatency",           0}
    };
    m_MemoryController.SetTimingParameters(timingMap);

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
