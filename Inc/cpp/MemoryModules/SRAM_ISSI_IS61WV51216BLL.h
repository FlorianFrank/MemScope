//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_SRAM_ISSI_IS61WV51216BLL_H
#define MEMORY_TESTING_FW_SRAM_ISSI_IS61WV51216BLL_H


#include "MemoryModule.h"

class SRAM_ISSI_IS61WV51216BLL: public MemoryModule
{
public:
    SRAM_ISSI_IS61WV51216BLL();
    MEM_ERROR Initialize() override;

protected:
    ~SRAM_ISSI_IS61WV51216BLL() override;
};


#endif //MEMORY_TESTING_FW_SRAM_ISSI_IS61WV51216BLL_H
