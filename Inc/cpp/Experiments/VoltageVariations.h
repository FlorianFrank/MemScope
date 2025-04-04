//
// Created by Florian Frank on 02.04.25.
//

#include "Reliability.h"

#pragma once

class VoltageVariations : public Reliability {
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
    MemoryErrorHandling::MEM_ERROR waitingForContinueMsg() ;

public:
    VoltageVariations(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper);
};


