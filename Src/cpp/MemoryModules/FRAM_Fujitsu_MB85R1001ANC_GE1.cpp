/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/FRAM_Fujitsu_MB85R1001ANC_GE1.h"

FRAM_Fujitsu_MB85R1001ANC_GE1::FRAM_Fujitsu_MB85R1001ANC_GE1() : MemoryModule(TYPE_FRAM, "FRAM_Fujitsu_MB85R1001ANC_GE1",
                                                                              MemoryProperties::PARALLEL, 8, 131072,
                                                                              {233, 358}, {3000, 3600})
{}

FRAM_Fujitsu_MB85R1001ANC_GE1::~FRAM_Fujitsu_MB85R1001ANC_GE1() = default;

MEM_ERROR FRAM_Fujitsu_MB85R1001ANC_GE1::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}
