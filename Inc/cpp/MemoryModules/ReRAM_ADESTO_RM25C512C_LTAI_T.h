//
// Created by florianfrank on 2021-12-23.
//

#ifndef MEMORY_TESTING_FW_RERAM_ADESTO_RM25C512C_LTAI_T_H
#define MEMORY_TESTING_FW_RERAM_ADESTO_RM25C512C_LTAI_T_H


#include "MemoryModule.h"

class SPIWrapper;

class RERAM_ADESTO_RM25C512C_LTAI_T: public MemoryModule
{
public:
    RERAM_ADESTO_RM25C512C_LTAI_T();
    ~RERAM_ADESTO_RM25C512C_LTAI_T() override;
    MEM_ERROR Initialize() override;

private:
    SPIWrapper *m_SPIWrapper;

};


#endif //MEMORY_TESTING_FW_RERAM_ADESTO_RM25C512C_LTAI_T_H
