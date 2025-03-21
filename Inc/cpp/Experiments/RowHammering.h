/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#pragma once

#include "MemoryExperiment.h"

/**
 * @file RowHammering.h
 * @brief Defines the RowHammering experiment for PUF evaluation.
 *
 * This class implements the RowHammering experiment, inheriting from the MemoryExperiment base class.
 * It conducts a memory hammering procedure to evaluate memory cell behavior under stress.
 *
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
class RowHammering: public MemoryExperiment {
public:
    RowHammering(MemoryController &memoryController, PUFConfiguration &pufConfig, InterfaceWrapper &interfaceWrapper);
    MemoryErrorHandling::MEM_ERROR init() override;
    MemoryErrorHandling::MEM_ERROR running() override;
    MemoryErrorHandling::MEM_ERROR done() override;
};