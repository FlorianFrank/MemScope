//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_FRAM_FUJITSU_MB85R1001ANC_GE1_H
#define MEMORY_TESTING_FW_FRAM_FUJITSU_MB85R1001ANC_GE1_H


#include "MemoryModule.h"

class FRAM_Fujitsu_MB85R1001ANC_GE1: public MemoryModule
{
public:
    MEM_ERROR Initialize() override;
    FRAM_Fujitsu_MB85R1001ANC_GE1();
    ~FRAM_Fujitsu_MB85R1001ANC_GE1() override;
};


#endif //MEMORY_TESTING_FW_FRAM_FUJITSU_MB85R1001ANC_GE1_H
