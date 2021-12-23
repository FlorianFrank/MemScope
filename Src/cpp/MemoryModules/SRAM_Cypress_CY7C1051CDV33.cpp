//
// Created by florianfrank on 2021-12-23.
//

#include "cpp/MemoryModules/SRAM_Cypress_CY7C1051CDV33.h"

SRAM_Cypress_CY7C1051CDV33::~SRAM_Cypress_CY7C1051CDV33() = default;

SRAM_Cypress_CY7C1051CDV33::SRAM_Cypress_CY7C1051CDV33(): MemoryModule(MemoryProperties::TYPE_SRAM,
"SRAM_Cypress_CY7C1051CDV33",
MemoryProperties::PARALLEL, 16, 262144,
{233, 358}, {3000, 3600})
{
}

MEM_ERROR SRAM_Cypress_CY7C1051CDV33::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}
