/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/SRAM_ISSI_IS61WV51216BLL.h"

SRAM_ISSI_IS61WV51216BLL::SRAM_ISSI_IS61WV51216BLL(): MemoryModule(MemoryProperties::TYPE_SRAM,
                                                                    "SRAM_ISSI_IS61WV51216BLL",
                                                                    MemoryProperties::PARALLEL, 16, 524288, {233, 358},
                                                                    {3000, 3600})
{}

SRAM_ISSI_IS61WV51216BLL::~SRAM_ISSI_IS61WV51216BLL() = default;

MEM_ERROR SRAM_ISSI_IS61WV51216BLL::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}




