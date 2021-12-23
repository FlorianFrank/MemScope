//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_FRAM_ROHM_MR48V256CTAZAARL_H
#define MEMORY_TESTING_FW_FRAM_ROHM_MR48V256CTAZAARL_H

#include "MemoryModule.h"

class FRAM_Rohm_MR48V256CTAZAARL: public MemoryModule
{
public:
    FRAM_Rohm_MR48V256CTAZAARL();
    ~FRAM_Rohm_MR48V256CTAZAARL() override;

    MEM_ERROR Initialize() override;

};


#endif //MEMORY_TESTING_FW_FRAM_ROHM_MR48V256CTAZAARL_H
