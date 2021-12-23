//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_MRAM_EVERSPIN_MR4A08BUYS45_H
#define MEMORY_TESTING_FW_MRAM_EVERSPIN_MR4A08BUYS45_H

#include "MemoryModule.h"

class MRAM_Everspin_MR4A08BUYS45: public MemoryModule
{
public:
    MRAM_Everspin_MR4A08BUYS45();
    ~MRAM_Everspin_MR4A08BUYS45() override;
    MEM_ERROR Initialize() override;

};


#endif //MEMORY_TESTING_FW_MRAM_EVERSPIN_MR4A08BUYS45_H
