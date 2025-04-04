//
// Created by Florian Frank on 02.04.25.
//

#include "cpp/Experiments/VoltageVariations.h"
#include "Logger.h"

constexpr uint32_t CONTINUE_TIMEOUT = 2000;
constexpr uint32_t CONTINUE_BUF_SIZE = 20;


VoltageVariations::VoltageVariations(MemoryController &memoryController, PUFConfiguration &pufConfig,
                                     InterfaceWrapper &interfaceWrapper) : Reliability(
        memoryController, pufConfig, interfaceWrapper) {
}

/**
 * @brief Initializes the Reliability experiment.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR VoltageVariations::init() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Initialize Voltage Variations Experiment -> Initialize with default timing and value 0x%x",
                m_PUFConfiguration.generalConfig.initValue);
    auto ret = initializeMemory(m_PUFConfiguration.generalConfig.initValue);
    if(ret != MEM_ERROR::MEM_NO_ERROR){
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__,
                    "initializeMemory failed with error code %s", MemoryErrorHandling::memErrorToString(ret));
        return ret;
    }
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Send response init message");
    return MemoryExperiment::sendCurrentState("R:init\n");
}

/**
 * @brief Runs the Reliability experiment.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR VoltageVariations::running() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Wait for continue");
    auto retInit = initializeMemory(m_PUFConfiguration.generalConfig.pufValue);
    if (retInit == MEM_ERROR::MEM_NO_ERROR) {
        Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Send response run message");
        return MemoryExperiment::sendCurrentState("R:run\n");
    } else {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "initializeMemory failed!");
        return retInit;
    }
}

/**
 * @brief Finalizes the Reliability experiment and verifies written results.
 * @return Memory error code.
 */
MemoryErrorHandling::MEM_ERROR VoltageVariations::done() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Read under normal voltage");
    auto retDone = Reliability::done();
    if (retDone != MEM_ERROR::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "Reliability::done() failed");
        return retDone;
    } else {
        return MemoryExperiment::sendCurrentState("R:done");
    }
}

/**
 * @brief Waits for a continue message by receiving data.
 *
 * This function waits for a specific message to be received using the
 * interface wrapper. It logs the waiting status, attempts to receive data
 * in blocking mode with a timeout of 10 ms, and handles any memory errors.
 *
 * @return MemoryErrorHandling::MEM_ERROR Returns MEM_NO_ERROR on success or
 *         an appropriate memory error code if receiving data fails.
 */
MemoryErrorHandling::MEM_ERROR VoltageVariations::waitingForContinueMsg() {
    uint16_t size = CONTINUE_BUF_SIZE;
    uint8_t recvData[CONTINUE_BUF_SIZE];
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Waiting for receive message");
    auto ret = m_InterfaceWrapper.ReceiveData(recvData, &size, BlockingMode::NON_BLOCKING, CONTINUE_TIMEOUT);
    if (ret != MEM_ERROR::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "Error while waiting for receive message");
        return ret;
    }

    return MemoryErrorHandling::MEM_NO_ERROR;
}
