/**
 * @file RowHammering.cpp
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 * @brief Implementation of Row Hammering experiment for PUF-based memory analysis.
 */

#include "cpp/Experiments/RowHammering.h"
#include "Logger.h"
#include "cpp/MemoryControllers/MemoryController.h"

/**
 * @brief Constructs a RowHammering object.
 * @param memoryController Reference to the MemoryController instance.
 * @param pufConfig Reference to the PUFConfiguration instance.
 * @param interfaceWrapper Reference to the InterfaceWrapper instance.
 */
RowHammering::RowHammering(MemoryController &memoryController, PUFConfiguration &pufConfig,
                           InterfaceWrapper &interfaceWrapper) : MemoryExperiment(
        memoryController, pufConfig, interfaceWrapper) {
}

/**
 * @brief Initializes the Row Hammering experiment.
 * @return MemoryErrorHandling::MEM_ERROR Error code indicating initialization status.
 */
MemoryErrorHandling::MEM_ERROR RowHammering::init() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Row Hammering -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory();
}

/**
 * @brief Executes the Row Hammering procedure.
 * @return MemoryErrorHandling::MEM_ERROR Error code indicating execution status.
 */
MemoryErrorHandling::MEM_ERROR RowHammering::running() {
    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto hammeringDistance = m_PUFConfiguration.rowHammeringConfig.hammeringDistance;
    auto hammeringIterations = m_PUFConfiguration.rowHammeringConfig.hammeringIterations;
    auto tWaitBetweenHammering = m_PUFConfiguration.rowHammeringConfig.tWaitBetweenHammering;

    auto pufValue = m_PUFConfiguration.generalConfig.pufValue;
    auto initValue = m_PUFConfiguration.generalConfig.initValue;
    auto bitWidth = m_MemoryController.getMemoryModule().GetBitWidth();

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Perform Row Hammering in address range [0x%x,0x%x] with init value 0x%x and row hammering parameters "
                "{hammering distance: 0x%x, hammering iterations: 0x%x, wait between hammering: 0x%x, hammering value 0x%x}",
                startAddress, endAddress, initValue, hammeringDistance, hammeringIterations, tWaitBetweenHammering,
                pufValue);

    // Outer loop iterates over the segments to which row hammering should be applied
    for (uint32_t startAddrRange = startAddress;
         startAddrRange <= endAddress + 1; startAddrRange += (2 * hammeringDistance)) {
        uint32_t hammerDistDynamic = (startAddrRange + 2 * hammeringDistance > (endAddress + 1))
                                     ? (endAddress + 1) - startAddrRange
                                     : hammeringDistance;
        for (uint32_t iteration = 0; iteration < hammeringIterations; iteration++) {
            for (uint32_t addressOffset = 0; addressOffset < hammerDistDynamic; addressOffset++) {
                if (bitWidth == 8) {
                    m_MemoryController.Write8BitWord(startAddrRange + addressOffset, pufValue);
                } else if (bitWidth == 16) {
                    m_MemoryController.Write16BitWord(startAddrRange + addressOffset, pufValue);
                }
            }
        }
    }

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Row Hammering performed");

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Concludes the Row Hammering experiment and retrieves memory contents.
 * @return MemoryErrorHandling::MEM_ERROR Error code indicating completion status.
 */
MemoryErrorHandling::MEM_ERROR RowHammering::done() {
    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto hammeringDistance = m_PUFConfiguration.rowHammeringConfig.hammeringDistance;
    auto bitWidth = m_MemoryController.getMemoryModule().GetBitWidth();

    char sendDataBuf[128];

    /**
     * @brief Sends read data to the interface.
     * @param interfaceWrapper Reference to the InterfaceWrapper instance.
     * @param addr Memory address.
     * @param returnValue Value read from memory.
     */
    auto sendData = [&sendDataBuf](InterfaceWrapper &interfaceWrapper, uint16_t addr, uint16_t returnValue) {
        int actualSize = snprintf(sendDataBuf, 128, "m:%x,%x,%x\n", addr, returnValue, addr + returnValue);
        if (actualSize > 0) {
            uint16_t size = actualSize;
            interfaceWrapper.SendData(reinterpret_cast<uint8_t *>(sendDataBuf), &size, 1000, false);
        } else {
            Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "snprintf returned error %s", strerror(errno));
        }
    };

    for (uint32_t startAddrRange = startAddress + hammeringDistance;
         startAddrRange <= endAddress + 1; startAddrRange += (2 * hammeringDistance)) {
        for (uint32_t addressOffset = 0; addressOffset < hammeringDistance; addressOffset++) {
            if (bitWidth == 8) {
                uint8_t returnValue;
#ifdef DEBUG_ENABLE
                Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Read from 0x%x", startAddrRange + addressOffset);
#endif
                m_MemoryController.Read8BitWord(startAddrRange + addressOffset, &returnValue);
                sendData(m_InterfaceWrapper, startAddrRange + addressOffset, returnValue);
            } else if (bitWidth == 16) {
                uint16_t returnValue;
#ifdef DEBUG_ENABLE
                Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Read from 0x%x", startAddrRange + addressOffset);
#endif
                m_MemoryController.Read16BitWord(startAddrRange + addressOffset, &returnValue);
                sendData(m_InterfaceWrapper, startAddrRange + addressOffset, returnValue);
            }
        }
    }
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Reading done");
    sendMessageFinished();
    return MemoryErrorHandling::MEM_NO_ERROR;
}
