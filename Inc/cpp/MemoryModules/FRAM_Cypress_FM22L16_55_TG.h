//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_FRAM_CYPRESS_FM22L16_55_TG_H
#define MEMORY_TESTING_FW_FRAM_CYPRESS_FM22L16_55_TG_H

#include "MemoryModule.h"

class FRAM_Cypress_FM22L16_55_TG: public MemoryModule
{
public:
    FRAM_Cypress_FM22L16_55_TG();
    ~FRAM_Cypress_FM22L16_55_TG() override;

    MEM_ERROR Initialize() override;

};


#endif //MEMORY_TESTING_FW_FRAM_CYPRESS_FM22L16_55_TG_H
