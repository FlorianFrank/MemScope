//
// Created by florianfrank on 07.12.21.
//

#include "cpp/MemoryControllerWrappers/MemoryControllerParallel.h"

MemoryControllerParallel::MemoryControllerParallel(InterfaceWrappers *interfaceWrapper) : MemoryController(
        interfaceWrapper)
{

}

MEM_ERROR MemoryControllerParallel::Write8BitWord(uint32_t adr, uint8_t value)
{
    if (isInvalidAddress(adr))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    *(__IO uint8_t *) (m_MMIOStartAddress + adr) = value;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerParallel::Read8BitWord(uint32_t adr, uint8_t *ret) const
{
    if (isInvalidAddress(adr))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if(!ret)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    *ret = *(__IO uint8_t *) (m_MMIOStartAddress + adr);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*
 * @brief					writes a 16-Bit(2-byte-word) value to the specified address to SRAM
 * @param uint32_t adr		relative address to the base address (specified as macro 'MEMORY_BANK_ADDRESS') of SRAM
 * 							to be written to
 * @param uint16_t value	value to be written to the specified address in SRAM
 * @retval					None
 */
MEM_ERROR MemoryControllerParallel::Write16BitWord(uint32_t adr, uint16_t value)
{
    if(isInvalidAddress(adr))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;


    // multiply address by 2 due to 2-byte-access (address is given as one byte)
    adr = adr << 1;

    *(__IO uint16_t *) (MEMORY_BANK_ADDRESS + adr) = value;
    return MemoryErrorHandling::MEM_NO_ERROR;

}

MEM_ERROR MemoryControllerParallel::Read16BitWord(uint32_t adr, uint16_t *value) const
{
    if(isInvalidAddress(adr))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if(!value)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // multiply address by 2 due to 2-byte-access (address is given as one byte)
    adr = adr << 1;

    *value = *(__IO uint16_t *) (m_MMIOStartAddress + adr);

    return MemoryErrorHandling::MEM_NO_ERROR;
}


