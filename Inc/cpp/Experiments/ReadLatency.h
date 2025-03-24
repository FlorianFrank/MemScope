/**
 * @class WriteLatency
 *
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#pragma once

#include "MemoryExperiment.h"

/**
 * @brief Implements a memory experiment to measure read latency under different timing configurations.
 */
class ReadLatency: public MemoryExperiment {
public:
    ReadLatency(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper);
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
};