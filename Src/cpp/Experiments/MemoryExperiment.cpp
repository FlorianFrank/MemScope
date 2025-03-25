/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/Experiments/MemoryExperiment.h"
//
// Created by florianfrank on 02.07.21.
//

#include <cstddef>
#include <cstdlib>
//#include "cpp/CommandLineParser.h"
#include <ctime>
//#include "usb_handler.h"
#include <meta_data_handler.h>
#include <cpp/MemoryControllers/MemoryController.h>
//#include "usb_defines.h"
#include "cpp/Experiments/MemoryExperiment.h"
#include "cpp/TimeMeasurement.h"
#include "Logger.h"
#include "cpp/Experiments/RowHammering.h"
#include "cpp/Experiments/Reliability.h"
#include "cpp/Experiments/ReadLatency.h"
#include "cpp/Experiments/WriteLatency.h"

/*static*/ std::vector<MemoryExperiment *> MemoryExperiment::experiments = {};

/*static*/ MemoryErrorHandling::MEM_ERROR
MemoryExperiment::MemoryTestFactory(MemoryExperiment **experiment, MemoryController &memoryController,
                                    PUFConfiguration &pufConfiguration, InterfaceWrapper &interfaceWrapper) {
    memoryController.Initialize();
    if (pufConfiguration.generalConfig.pufType == ROW_HAMMERING) {
        *experiment = new RowHammering(memoryController, pufConfiguration, interfaceWrapper);
        (*experiment)->configureMemoryController();
        experiments.push_back(*experiment);
    } else if (pufConfiguration.generalConfig.pufType == RELIABLE) {
        *experiment = new Reliability(memoryController, pufConfiguration, interfaceWrapper);
        (*experiment)->configureMemoryController();
        experiments.push_back(*experiment);
    } else if (pufConfiguration.generalConfig.pufType == READ_LATENCY) {
        *experiment = new ReadLatency(memoryController, pufConfiguration, interfaceWrapper);
        (*experiment)->configureMemoryController();
        experiments.push_back(*experiment);
    } else if (pufConfiguration.generalConfig.pufType == WRITE_LATENCY) {
        *experiment = new WriteLatency(memoryController, pufConfiguration, interfaceWrapper);
        (*experiment)->configureMemoryController();
        experiments.push_back(*experiment);
    } else {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "%s",
                    MemoryErrorHandling::memErrorToString(MemoryErrorHandling::EXPERIMENT_NOT_SUPORTED));
        return MemoryErrorHandling::EXPERIMENT_NOT_SUPORTED;
    }

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*static*/ MemoryErrorHandling::MEM_ERROR MemoryExperiment::deleteExperiments() {
    for (auto experiment: experiments) {
        delete experiment;
    }
    MemoryExperiment::experiments.clear();
    return MemoryErrorHandling::MEM_ERROR::MEM_NO_ERROR;
}

MemoryExperiment::MemoryExperiment(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper) :
        m_MemoryController(memoryController), m_PUFConfiguration(pufConfig), m_InterfaceWrapper(interfaceWrapper) {
}

MemoryErrorHandling::MEM_ERROR MemoryExperiment::configureMemoryController() {
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Set the memories default timing parameters");
    // TODO change back to normal timing
    std::map<std::string, uint16_t> timingMap = {
            {"addressSetupTime",      m_PUFConfiguration.readTimingConfigAdjusted.tAS},
            {"addressHoldTime",       m_PUFConfiguration.readTimingConfigAdjusted.tAH},
            {"dataSetupTime",         m_PUFConfiguration.readTimingConfigAdjusted.tPRC},
            {"busTurnAroundDuration", 4},
            {"clkDivision",           8},
            {"dataLatency",           0}
    };

    m_MemoryController.SetTimingParameters(timingMap);
    return MemoryErrorHandling::MEM_ERROR::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR MemoryExperiment::initializeMemory() {
    sendProcessingStarted();
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Fill memory area [0x%x,0x%x], with value 0x%x",
                m_PUFConfiguration.generalConfig.startAddress, m_PUFConfiguration.generalConfig.endAddress,
                m_PUFConfiguration.generalConfig.initValue);
    auto ret = m_MemoryController.FillMemoryArea(m_PUFConfiguration.generalConfig.startAddress,
                                                 m_PUFConfiguration.generalConfig.endAddress,
                                                 static_cast<uint16_t>(m_PUFConfiguration.generalConfig.initValue) &
                                                 0xFFFF);
    return ret;
}

/**
 * @brief Sends a processing started message over the interface.
 *
 * Logs the processing start and sends a predefined "R:processing" message.
 */
void MemoryExperiment::sendProcessingStarted() {
    const char *processingResponse = "R:processing\n";
    uint16_t processingSize = strlen(processingResponse);
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Config parsed -> send response %s", processingResponse);
    m_InterfaceWrapper.SendData(reinterpret_cast<uint8_t *>(const_cast<char *>(processingResponse)), &processingSize, 1000, true);
}

/**
 * @brief Sends a message indicating the experiment has finished.
 *
 * Logs the experiment completion and sends a predefined "R:ready" message.
 */
void MemoryExperiment::sendMessageFinished() {
    const char *finalResponse = "R:ready\n";
    uint16_t finalLen = strlen(finalResponse);
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Experiment done send %s", finalResponse);
    m_InterfaceWrapper.SendData(reinterpret_cast<uint8_t *>(const_cast<char *>(finalResponse)), &finalLen, 1000, true);
}
