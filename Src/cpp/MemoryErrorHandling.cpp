/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef SRC_MEMORY_ERROR_HANDLING_C_
#define SRC_MEMORY_ERROR_HANDLING_C_

#include "cpp/MemoryErrorHandling.h"


#define HAL_OK       0x00U
#define HAL_ERROR    0x01U
#define HAL_BUSY     0x02U
#define HAL_TIMEOUT  0x03U

/*static*/ MemoryErrorHandling::MEM_ERROR MemoryErrorHandling::HAL_StatusTypeDefToErr(int status)
{
    switch(status)
    {
        case (HAL_OK):
            return MEM_NO_ERROR;
        case (HAL_ERROR):
            return MEM_HAL_INTERNAL_ERROR;
        case (HAL_BUSY):
            return MEM_HAL_BUSY;
        case (HAL_TIMEOUT):
            return MEM_HAL_TIMEOUT;
        default:
            return MEM_UNKNOWN_ERROR;
    }
}

/*static*/ const char * MemoryErrorHandling::MemErrorToString(MEM_ERROR errorCode)
{
    switch (errorCode)
    {
        case MEM_NO_ERROR:
            return "No Error occured";
        case MEM_REGISTER_NOT_SET:
            return "Register could not be written";
        case MEM_UNKNOWN_ERROR:
            return "Unknown Error";
        case MEM_TIMEOUT:
            return "A timeout occurred";
        case MEM_HAL_INTERNAL_ERROR:
            return "Internal error";
        case MEM_HAL_BUSY:
            return "Function is busy";
        case MEM_HAL_TIMEOUT:
            return "Timeout ocurred";
        case MEM_INTERFACE_NOT_SUPPORTED:
            return "Memory interface not supported";
        case MEM_INTERFACE_NOT_FOUND:
            return "Interface not found";
        case MEM_INVALID_ARGUMENT:
            return "Invalid argument passed to function";
        case MEM_IO_ERROR:
            return "Error during I/O operation";
        case MEM_INVALID_ADDRESS:
            return "Error invalid address was used";
        case MEM_NOT_WRITTEN:
            return "Memory was not written";
        case MEM_UNEXPECTED_VALUE:
            return "Memory returns unexpected value";
        case MEM_INVALID_COMMAND:
            return "Invalid command";
        case MEM_BUFFER_TO_SMALL:
            return "Buffer to small";
        case MEM_MEMORY_NOT_SUPPORTED:
            return "Memory type not supported";
    }
}


#endif /* SRC_MEMORY_ERROR_HANDLING_C_ */
