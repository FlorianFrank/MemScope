/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/MRAM_Everspin_MR4A08BUYS45.h"


MRAM_Everspin_MR4A08BUYS45::MRAM_Everspin_MR4A08BUYS45(): MemoryModule(MemoryProperties::TYPE_MRAM,
                                                                        "MRAM_Everspin_MR4A08BUYS45",
                                                                        MemoryProperties::PARALLEL, 8, 2097152,
                                                                        {233, 358}, {3000, 3600})
{}

MRAM_Everspin_MR4A08BUYS45::~MRAM_Everspin_MR4A08BUYS45() = default;

MEM_ERROR MRAM_Everspin_MR4A08BUYS45::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}