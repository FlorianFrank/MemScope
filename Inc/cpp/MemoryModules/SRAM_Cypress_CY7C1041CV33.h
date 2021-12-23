//
// Created by florianfrank on 23.12.21.
//

#ifndef MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
#define MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1041CV33_H


#include "MemoryModule.h"

class SRAM_Cypress_CY7C1041CV33 : public MemoryModule
{
public:
    SRAM_Cypress_CY7C1041CV33();
    ~SRAM_Cypress_CY7C1041CV33() override;

    MEM_ERROR Initialize() override;
};


#endif //MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
