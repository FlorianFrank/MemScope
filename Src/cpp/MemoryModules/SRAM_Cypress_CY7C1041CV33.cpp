/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/SRAM_Cypress_CY7C1041CV33.h"


SRAM_Cypress_CY7C1041CV33::SRAM_Cypress_CY7C1041CV33() : MemoryModule(MemoryProperties::TYPE_SRAM,
                                                                      "SRAM_Cypress_CY7C1041CV33",
                                                                      MemoryProperties::PARALLEL, 16, 262144,
                                                                      {233, 358}, {3000, 3600})
{}

SRAM_Cypress_CY7C1041CV33::~SRAM_Cypress_CY7C1041CV33() = default;

MEM_ERROR SRAM_Cypress_CY7C1041CV33::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}
