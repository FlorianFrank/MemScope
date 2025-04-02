//
// Created by Florian Frank on 02.04.25.
//

#include "cpp/Experiments/VolatageVariations.h"
#include "Logger.h"

VolatageVariations::VolatageVariations(MemoryController &memoryController, PUFConfiguration &pufConfig,
                         InterfaceWrapper &interfaceWrapper) : Reliability(
        memoryController, pufConfig, interfaceWrapper) {
}

/**
 * @brief Initializes the Reliability experiment.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR Reliability::init() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Voltage Variations Experiment -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    return initializeMemory(m_PUFConfiguration.generalConfig.initValue);
}

/**
 * @brief Runs the Reliability experiment.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR VolatageVariations::running() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Write memory with altered voltage");
    //m_InterfaceWrapper.ReceiveData(); TODO wait for acceptance that voltage was adjusted

    return initializeMemory(m_PUFConfiguration.generalConfig.pufValue);
}

/**
 * @brief Finalizes the Reliability experiment and verifies written results.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR VolatageVariations::done() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Read under normal voltage");
    return Reliability::done();
}
