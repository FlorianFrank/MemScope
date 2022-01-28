/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYERRORHANDLING_H
#define MEMORY_TESTING_FW_MEMORYERRORHANDLING_H


class MemoryErrorHandling
{
public:
    enum MEM_ERROR
    {
        MEM_NO_ERROR = 0,
        MEM_REGISTER_NOT_SET = 1,
        MEM_UNKNOWN_ERROR = 2,
        MEM_TIMEOUT = 3,
        MEM_HAL_INTERNAL_ERROR = 4,
        MEM_HAL_BUSY = 5,
        MEM_HAL_TIMEOUT = 6,
        MEM_INTERFACE_NOT_SUPPORTED = 7,
        MEM_INTERFACE_NOT_FOUND = 8,
        MEM_INVALID_ARGUMENT = 9,
        MEM_BIT_LEN_NOT_SUPPORTED = 10,
        MEM_IO_ERROR = 11,
        MEM_INVALID_ADDRESS = 12,
        MEM_NOT_WRITTEN = 13,
        MEM_UNEXPECTED_VALUE = 14,
        MEM_INVALID_COMMAND = 15,
        MEM_BUFFER_TO_SMALL = 16,
        MEM_MEMORY_NOT_SUPPORTED = 17,
        MEM_DEVICE_NOT_SPECIFIED = 18,
        MEM_MEMORY_CONTROLLER_NOT_SPECIFIED = 19,
        MEM_GPIO_PIN_ALREADY_IN_USE = 20,
        MEM_UNSUPPORTED_BAUDRATE = 21,
        MEM_FUNCTION_NOT_IMPLEMENTED = 22
    } typedef MEM_ERROR;

    static MEM_ERROR HAL_StatusTypeDefToErr(int status);
    static const char * MemErrorToString(MEM_ERROR errorCode);
};



#endif //MEMORY_TESTING_FW_MEMORYERRORHANDLING_H
