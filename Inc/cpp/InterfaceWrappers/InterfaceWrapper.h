/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_INTERFACEWRAPPER_H
#define MEMORY_TESTING_FW_INTERFACEWRAPPER_H

#include <cstdint>
#include "io_pin_defines.h"
#include "cpp/MemoryErrorHandling.h"

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class InterfaceWrapper
{
public:
    virtual MEM_ERROR Initialize() = 0;
    virtual ~InterfaceWrapper()= default;

    virtual MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) = 0;
    virtual MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) = 0;

protected:
    char STRING_BUFFER[STRING_BUFFER_SIZE]{};
    // receive buffer

};


#endif //MEMORY_TESTING_FW_INTERFACEWRAPPER_H
