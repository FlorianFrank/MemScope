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
        MEM_NO_ERROR,
        MEM_REGISTER_NOT_SET,
        MEM_UNKNOWN_ERROR,
        MEM_TIMEOUT,
        MEM_HAL_INTERNAL_ERROR,
        MEM_HAL_BUSY,
        MEM_HAL_TIMEOUT,
        MEM_INTERFACE_NOT_SUPPORTED,
        MEM_INVALID_ARGUMENT,
        MEM_IO_ERROR,
        MEM_INVALID_ADDRESS,
        MEM_NOT_WRITTEN,
        MEM_UNEXPECTED_VALUE,
        MEM_INVALID_COMMAND,
        MEM_BUFFER_TO_SMALL,
        MEM_MEMORY_NOT_SUPPORTED

    } typedef MEM_ERROR;

    static MEM_ERROR HAL_StatusTypeDefToErr(int status);
};



#endif //MEMORY_TESTING_FW_MEMORYERRORHANDLING_H
