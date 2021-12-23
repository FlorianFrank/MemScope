//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
#define MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H

#include "MemoryModule.h"

class SRAM_Cypress_CY7C1051CDV33: public MemoryModule
{
protected:
    SRAM_Cypress_CY7C1051CDV33();
    ~SRAM_Cypress_CY7C1051CDV33() override;

public:
    MEM_ERROR Initialize() override;
};


#endif //MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
