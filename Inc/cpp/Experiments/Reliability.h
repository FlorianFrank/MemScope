/**
 * @class WriteLatency
 *
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#pragma once

#include "MemoryExperiment.h"

/**
 * @brief Experiments tests if reliable reading and writing is possible under timing specifications according to the memory manual.
 */
class Reliability: public MemoryExperiment {
public:
    Reliability(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper);
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
};