//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1_H
#define MEMORY_TESTING_FW_RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1_H

#include "MemoryModule.h"

class SPIWrapper;

class ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1: public MemoryModule
{
public:
    ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1();
    ~ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1() override;

    MEM_ERROR Initialize() override;

private:
    SPIWrapper* m_SPIWrapper;
};


#endif //MEMORY_TESTING_FW_RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1_H
