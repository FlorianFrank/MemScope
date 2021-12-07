//
// Created by florianfrank on 07.12.21.
//

#include <cstdint>
#include <cstddef>
#include <cstring>
#include "cpp/InterfaceWrappers.h"

/*
* @brief					clears the rx_buffer starting from address specified in param index
* @param uint8_t index		index to start from clearing the buffer
*/
void InterfaceWrappers::clearBuffer(uint8_t index)
{
    for(uint8_t i = index; i < STRING_BUFFER_SIZE; i++){
        Rx_Buffer[i] = 0;
    }
}

bool InterfaceWrappers::isTransferCplt() const
{
        return Transfer_cplt;
}

void InterfaceWrappers::resetTransferCplt()
{
    Transfer_cplt = false;
}

uint8_t InterfaceWrappers::get_space(char *rx_buffer)
{
    for (size_t i = 0; i < strlen(rx_buffer); i++)
    {
        if (rx_buffer[i] == 32)
        {
            return i;
        }
    }
    return (uint8_t) strlen(rx_buffer);
}
