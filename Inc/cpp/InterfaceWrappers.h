//
// Created by florianfrank on 07.12.21.
//

#ifndef MEMORY_TESTING_FW_INTERFACEWRAPPERS_H
#define MEMORY_TESTING_FW_INTERFACEWRAPPERS_H

#include <cstdint>
#include "io_pin_defines.h"
#include "MemoryErrorHandling.h"

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class InterfaceWrappers
{
public:

    virtual MEM_ERROR SendData(uint8_t *data, const uint16_t *size, uint32_t timeout) = 0;
    virtual MEM_ERROR ReceiveData(uint8_t *data, const uint16_t *size, uint32_t timeout) = 0;

    // Helper functions
    bool isTransferCplt() const;
    void resetTransferCplt();
    uint8_t get_space(char *rx_buffer);

protected:
    void clearBuffer(uint8_t index);
    char STRING_BUFFER[STRING_BUFFER_SIZE];
    // receive buffer
    char Rx_Data[2];
    char Rx_Buffer[100];
    uint8_t Rx_Index;
    // receive buffer
    bool Transfer_cplt;
};


#endif //MEMORY_TESTING_FW_INTERFACEWRAPPERS_H
