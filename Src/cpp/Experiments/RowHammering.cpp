//
// Created by Florian Frank on 19.03.25.
//
#include "cpp/Experiments/RowHammering.h"
#include "Logger.h"
#include "cpp/MemoryControllers/MemoryController.h"


#define DEBUG_ENABLE 1

RowHammering::RowHammering(MemoryController &memoryController, PUFConfiguration &pufConfig) : MemoryExperiment(
        memoryController, pufConfig) {
}

MemoryErrorHandling::MEM_ERROR RowHammering::init() {

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Row Hammering -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory();
}

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
    for (uint32_t startAddrRange = startAddress; startAddrRange <= endAddress+1; startAddrRange += (2 * hammeringDistance)) {
        // Dynamically adjust the hammering distance to cover the last segment
        uint32_t hammerDistDynamic = (startAddrRange + 2 * hammeringDistance > (endAddress+1)) ? (endAddress+1) - startAddrRange : hammeringDistance;
        // Perform hammering iterations for the current range
        for (uint32_t iteration = 0; iteration < hammeringIterations; iteration++) {
            for (uint32_t addressOffset = 0; addressOffset < hammerDistDynamic; addressOffset++) {
                if (bitWidth == 8) {
#ifdef DEBUG_ENABLE
                    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Hammer 0x%x", startAddrRange + addressOffset);
#endif
                    m_MemoryController.Write8BitWord(startAddrRange + addressOffset, pufValue);
                } else if (bitWidth == 16) {
#ifdef DEBUG_ENABLE
                    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Hammer 0x%x", startAddrRange + addressOffset);
#endif
                    m_MemoryController.Write16BitWord(startAddrRange + addressOffset, pufValue);
                }
            }
        }
    }

    // Log completion of Row Hammering
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Row Hammering performed");

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR RowHammering::done() {

    auto startAddress = m_PUFConfiguration.generalConfig.startAddress;
    auto endAddress = m_PUFConfiguration.generalConfig.endAddress;
    auto hammeringDistance = m_PUFConfiguration.rowHammeringConfig.hammeringDistance;
    auto bitWidth = m_MemoryController.getMemoryModule().GetBitWidth();

    for (uint32_t startAddrRange = startAddress + hammeringDistance;
         startAddrRange <= endAddress + 1; startAddrRange += (2 * hammeringDistance)) {
        for (uint32_t addressOffset = 0; addressOffset < hammeringDistance; addressOffset++) {

            if (bitWidth == 8) {
                uint8_t returnValue;
#ifdef DEBUG_ENABLE
                Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Read from 0x%x", startAddrRange + addressOffset);
#endif
                m_MemoryController.Read8BitWord(startAddrRange + addressOffset, &returnValue);
            } else if (bitWidth == 16) {
                uint16_t returnValue;
#ifdef DEBUG_ENABLE
                Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Read from 0x%x", startAddrRange + addressOffset);
#endif
                m_MemoryController.Read16BitWord(startAddrRange + addressOffset, &returnValue);
            }
        }
    }

    // TODO pipe results outside!
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Reading done");
    return MemoryErrorHandling::MEM_REGISTER_NOT_SET;
}
