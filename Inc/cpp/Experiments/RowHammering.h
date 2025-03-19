#pragma once

#include "MemoryExperiment.h"

class RowHammering: public MemoryExperiment {
public:
    RowHammering(MemoryController &memoryController, PUFConfiguration &pufConfig);
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
};