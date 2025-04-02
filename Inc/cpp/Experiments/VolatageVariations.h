//
// Created by Florian Frank on 02.04.25.
//

#include "Reliability.h"

#pragma once

class VolatageVariations : public Reliability {
    VolatageVariations(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper);
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
};


