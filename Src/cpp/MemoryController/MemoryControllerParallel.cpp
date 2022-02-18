/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/MemoryControllers/MemoryControllerParallel.h"

#ifdef STM32
extern "C" {
#include <SystemFiles/fmc.h>
}
#endif // STM32

MemoryControllerParallel::MemoryControllerParallel(InterfaceWrapper *interfaceWrapper, MemoryModule &memoryModule,
                                                   DeviceWrapper &deviceWrapper)
        : MemoryController(deviceWrapper, interfaceWrapper, memoryModule)
{
}

MEM_ERROR MemoryControllerParallel::Initialize()
{

    auto initializePins =  [this] (const std::vector<GPIOPin>& gpioList) -> MEM_ERROR {
        MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
        for (const auto& pin : gpioList)
        {
            ret = m_DeviceWrapper.InitializeGPIOPin(pin, GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                                    {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
            if (ret != MemoryErrorHandling::MEM_NO_ERROR)
              return ret;
        }
        return ret;
    };

    MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
    ret = initializePins(m_Properties.GetAddressLinesList());
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = initializePins(m_Properties.GetDataLinesList());
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetWE(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetCS(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetLB(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetUB(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetOE(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

#if STM32
    MX_FMC_Init();
#endif // STM32
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function writes a 8 bit value to the passed address.
 * Currently only a STM32F429DISC1 board is supported, which writes values using memory mapped IO.
 * @param adr address to write the value.
 * @param value the value to write to the specified address.
 * @return MEM_NO_ERROR if write execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Write8BitWord(uint32_t adr, uint8_t value)
{
    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize()))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

#if STM32
    *(__IO uint8_t *) (m_MMIOStartAddress + adr) = value;
#endif // STM32
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function reads an 8 bit value from a passed address.
 * Currently only a STM32F429DISC1 board is supported, which reads values using memory mapped IO.
 * @param adr address to read from.
 * @param ret the value which is returned after the reading operation.
 * @return MEM_NO_ERROR if write execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Read8BitWord(uint32_t adr, uint8_t *ret)
{
    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize()))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if(!ret)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

#if STM32
    *ret = *(__IO uint8_t *) (m_MMIOStartAddress + adr);
#endif // STM32

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function writes a 16 bit value to the passed address.
 * Currently only a STM32F429DISC1 board is supported, which writes values using memory mapped IO.
 * @param adr address to write the value.
 * @param value the value to write to the specified address.
 * @return MEM_NO_ERROR if write execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Write16BitWord(uint32_t adr, uint16_t value)
{
    if(IsInvalidAddress(adr, m_MemoryModule.GetMemorySize()))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;


    // multiply address by 2 due to 2-byte-access (address is given as one byte)
    adr = adr << 1;

#if STM32
    *(__IO uint16_t *) (MEMORY_BANK_ADDRESS + adr) = value;
#endif // STM32

    return MemoryErrorHandling::MEM_NO_ERROR;

}

/**
 * @brief This function reads an 16 bit value from a passed address.
 * Currently only a STM32F429DISC1 board is supported, which reads values using memory mapped IO.
 * @param adr address to read from.
 * @param value the value to be read. Add a pointer as argument to retrieve the data.
 * @return MEM_NO_ERROR if write execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Read16BitWord(uint32_t adr, uint16_t *value)
{
    if(IsInvalidAddress(adr, m_MemoryModule.GetMemorySize()))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if(!value)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // multiply address by 2 due to 2-byte-access (address is given as one byte)
    adr = adr << 1;

#if STM32
    *value = *(__IO uint16_t *) (m_MMIOStartAddress + adr);
#endif // STM32

    return MemoryErrorHandling::MEM_NO_ERROR;
}

