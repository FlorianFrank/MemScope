//
// Created by florianfrank on 09.12.21.
//

#include <cstring>
#include "TestInterfaceWrapper.h"

MemoryErrorHandling::MEM_ERROR TestInterfaceWrapper::StoreBuffer(uint8_t *buffer, const uint16_t *bufferSize)
{
    if(*bufferSize > 1024)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    memcpy(m_Buffer, buffer, *bufferSize);
    m_CurrentSize = *bufferSize;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR TestInterfaceWrapper::ReadBuffer(uint8_t *buffer, uint16_t *bufferSize)
{
    if(!buffer)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    uint32_t memoryToRead = (*bufferSize > m_CurrentSize) ? m_CurrentSize : *bufferSize;
    memcpy(buffer, m_Buffer, memoryToRead);
    *bufferSize = memoryToRead;

    return MemoryErrorHandling::MEM_NO_ERROR;
}
