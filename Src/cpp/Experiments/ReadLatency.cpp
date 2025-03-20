//
// Created by Florian Frank on 19.03.25.
//
#include "cpp/Experiments/ReadLatency.h"
#include "Logger.h"
#include "cpp/MemoryControllers/MemoryController.h"

#include <string.h>


ReadLatency::ReadLatency(MemoryController &memoryController, PUFConfiguration &pufConfig,
                         InterfaceWrapper &interfaceWrapper) : MemoryExperiment(
        memoryController, pufConfig, interfaceWrapper) {
}

MemoryErrorHandling::MEM_ERROR ReadLatency::init() {

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Read Latency Experiment -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory();
}

MemoryErrorHandling::MEM_ERROR ReadLatency::running() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Do not perform any action, as memory initialized in init function");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR ReadLatency::done() {

    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto initValue = m_PUFConfiguration.generalConfig.initValue;

    // TODO change back to normal timing
    std::map<std::string, uint16_t> timingMap = {
            {"addressSetupTime",      m_PUFConfiguration.readTimingConfigAdjusted.tAS},
            {"addressHoldTime",       m_PUFConfiguration.readTimingConfigAdjusted.tAH},
            {"dataSetupTime",         m_PUFConfiguration.writeTimingConfigDefault.tDS},
            {"busTurnAroundDuration", m_PUFConfiguration.writeTimingConfigDefault.tPWD},
            {"clkDivision",           16},
            {"dataLatency",           m_PUFConfiguration.readTimingConfigAdjusted.tPRC}
    };

    m_MemoryController.SetTimingParameters(timingMap);


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


    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Read values with reduced timing in range [0x%x,0x%x] ",
                startAddress, endAddress, initValue);

    for (auto addr = startAddress; addr < endAddress; addr++) {
        if (m_MemoryController.getMemoryModule().GetBitWidth() == 8) {
            uint8_t readValue;
            m_MemoryController.Read8BitWord(addr, &readValue);
            sendData(m_InterfaceWrapper, addr, readValue);
        } else if (m_MemoryController.getMemoryModule().GetBitWidth() == 16) {
            uint16_t readValue;
            m_MemoryController.Read16BitWord(addr, &readValue);
            sendData(m_InterfaceWrapper, addr, readValue);
        } else {
            Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Bit width %d not supported",
                        m_MemoryController.getMemoryModule().GetBitWidth());
            return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
        }
    }

    // TODO change back to normal timing
    std::map<std::string, uint16_t> timingMapDefault = {
            {"addressSetupTime",      m_PUFConfiguration.readTimingConfigDefault.tAS},
            {"addressHoldTime",       m_PUFConfiguration.readTimingConfigDefault.tAH},
            {"dataSetupTime",         m_PUFConfiguration.writeTimingConfigDefault.tDS},
            {"busTurnAroundDuration", m_PUFConfiguration.writeTimingConfigDefault.tPWD},
            {"clkDivision",           16},
            {"dataLatency",           m_PUFConfiguration.readTimingConfigDefault.tPRC}
    };
    m_MemoryController.SetTimingParameters(timingMapDefault);

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Reading done");
    return MemoryErrorHandling::MEM_NO_ERROR;
}