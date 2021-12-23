//
// Created by florianfrank on 23.12.21.
//

#ifndef MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C38098A_H
#define MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C38098A_H

#include "MemoryModule.h"

class SRAM_Alliance_AS7C38098A: public MemoryModule
{
    ~SRAM_Alliance_AS7C38098A() override;

    SRAM_Alliance_AS7C38098A();
    MEM_ERROR Initialize() override;
};


#endif //MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C38098A_H
