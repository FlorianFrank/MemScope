#pragma once

#include "MemoryExperiment.h"

class WriteLatency: public MemoryExperiment {
public:
    WriteLatency(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper);
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
};