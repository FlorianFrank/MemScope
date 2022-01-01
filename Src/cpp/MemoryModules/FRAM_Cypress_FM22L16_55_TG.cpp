/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/FRAM_Cypress_FM22L16_55_TG.h"

FRAM_Cypress_FM22L16_55_TG::FRAM_Cypress_FM22L16_55_TG() : MemoryModule(MemoryProperties::TYPE_FRAM,
                                                                        "FRAM_Cypress_FM22L16_55_TG",
                                                                        MemoryProperties::PARALLEL, 16, 262144,
                                                                        {233, 358}, {3000, 3600})
{}

FRAM_Cypress_FM22L16_55_TG::~FRAM_Cypress_FM22L16_55_TG() = default;

MEM_ERROR FRAM_Cypress_FM22L16_55_TG::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}
