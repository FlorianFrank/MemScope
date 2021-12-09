//
// Created by florianfrank on 09.12.21.
//

#ifndef MEMORY_TESTING_FW_TESTINTERFACEWRAPPER_H
#define MEMORY_TESTING_FW_TESTINTERFACEWRAPPER_H

#include <cstdint>
#include "cpp/MemoryErrorHandling.h"

#define BUFF_SIZE 1024

class TestInterfaceWrapper
{
public:
    MemoryErrorHandling::MEM_ERROR StoreBuffer(uint8_t *buffer, const uint16_t *bufferSize);
    MemoryErrorHandling::MEM_ERROR ReadBuffer(uint8_t *buffer, uint16_t *bufferSize);

private:
    uint8_t m_Buffer[BUFF_SIZE];
    uint32_t m_CurrentSize;
};


#endif //MEMORY_TESTING_FW_TESTINTERFACEWRAPPER_H
