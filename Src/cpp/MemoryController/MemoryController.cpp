/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/MemoryControllers/MemoryController.h"
#include <CommandDefines.h>

#include "io_pin_defines.h"
extern "C" {
#include "metrics.h"
}
#include <cstring> // strstr, memset
#include <cstdio> // sprintf


#include "cpp/InterfaceWrappers/SPIWrapper.h"

#define MEM_ACCESS_WIDTH_BIT 16 // TODO

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;


MemoryController::MemoryController(InterfaceWrapper *interfaceWrapper, uint32_t memorySize) : write_mode(0xFF), len(0), STRING_BUFFER(""),
                                                                                              m_MMIOStartAddress(MEMORY_BANK_ADDRESS),
                                                                                              srambp(nullptr), total_one(0), total_zero(0),
                                                                                              flipped_one(0), flipped_zero(0), m_MemorySize(memorySize)
{
    m_InterfaceWrapper = interfaceWrapper;
}

MemoryController::~MemoryController()
{

}

MEM_ERROR MemoryController::WriteSingleValue(uint32_t address, uint8_t value)
{
    return Write8BitWord(address, value);
}

MEM_ERROR MemoryController::WriteSingleValue(uint32_t address, uint16_t value)
{
    return Write16BitWord(address, value);
}

MEM_ERROR MemoryController::FillMemoryArea(uint32_t startAddress, uint32_t endAddress, uint8_t value)
{
    for(uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        MEM_ERROR ret = Write8BitWord(adr, value);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

// TODO überladung
MEM_ERROR MemoryController::FillMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t value)
{
    for (uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        MEM_ERROR ret = Write16BitWord(adr, value);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR
MemoryController::FillMemoryArea(uint32_t startAddress, uint32_t endAddress, uint8_t(*dataFunction)(uint32_t))
{
    for(uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        MEM_ERROR ret = Write8BitWord(adr, dataFunction(adr));
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}


MEM_ERROR
MemoryController::FillMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t(*dataFunction)(uint32_t))
{
    for(uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        MEM_ERROR ret = Write16BitWord(adr, dataFunction(adr));
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}


MEM_ERROR MemoryController::VerifyMemoryArea(uint32_t startAddress, uint32_t endAddress, uint8_t expectedValue)
{
    for (uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        uint8_t trueValue = 0;
        MEM_ERROR ret = Read8BitWord(adr, &trueValue);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        if(trueValue != expectedValue)
            return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}


MEM_ERROR MemoryController::VerifyMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t expectedValue)
{
    for (uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        uint16_t trueValue = 0;
        MEM_ERROR ret = Read16BitWord(adr, &trueValue);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        if(trueValue != expectedValue)
            return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryController::VerifyMemoryArea(uint32_t startAddress, uint32_t endAddress, uint8_t(*dataFunction)(uint32_t))
{
    for (uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        uint8_t trueValue = 0;
        MEM_ERROR ret = Read8BitWord(adr, &trueValue);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        if(trueValue != dataFunction(adr))
            return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryController::VerifyMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t(*dataFunction)(uint32_t))
{
    for (uint32_t adr = startAddress; adr < endAddress; adr++)
    {
        uint16_t trueValue = 0;
        MEM_ERROR ret = Read16BitWord(adr, &trueValue);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        if(trueValue != dataFunction(adr))
            return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}


/*
 * @brief								fills the whole SRAM with 0's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillWithZeros(){
        // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t value = 0x0000;
#elif MEM_ACCESS_WIDTH_BIT == 8
    uint8_t value = 0x00;
#endif // MEM_ACCESS_WIDTH_BIT == 16

    return FillMemoryArea(0, m_MemorySize, value);
}


MEM_ERROR MemoryController::FillWithZerosAndVerifyRead()
{
#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t value = 0x0000;
#elif MEM_ACCESS_WIDTH_BIT == 8
    uint8_t value = 0x00;
#endif // MEM_ACCESS_WIDTH_BIT == 16
    auto ret = FillMemoryArea(0, m_MemorySize, value);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    return VerifyMemoryArea(0, m_MemorySize, value);
}


/*
 * @brief								fills the whole SRAM with 1's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillWithOnes()
{
    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t value = 0xFFFF;
#elif MEM_ACCESS_WIDTH_BIT == 8
    uint8_t value = 0xFF;
#endif // MEM_ACCESS_WIDTH_BIT == 16

    return FillMemoryArea(0, m_MemorySize, value);
}

/*
 * @brief								fills the first address SRAM with the argument and increment it by one
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the start value to count up
 */
MEM_ERROR MemoryController::FillMemoryWithAscendingValues()
{
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16
    auto countFunction = [](uint32_t address)
    {
        return static_cast<uint16_t>(address % 0xFFFF);
    };

#elif MEM_ACCESS_WIDTH_BIT == 8
    auto countFunction = [](uint32_t address)
    {
        return static_cast<uint8_t>(address % 0xFF);
    };
#endif // MEM_ACCESS_WIDTH_BIT == 16
    auto ret = FillMemoryArea(0, m_MemorySize, countFunction);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR
MemoryController::FillMemoryWithAscendingValuesAndVerifyRead(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments)
{
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16
    auto countFunction = [](uint32_t address)
    {
        return static_cast<uint16_t>(address % 0xFFFF);
    };

#elif MEM_ACCESS_WIDTH_BIT == 8
    auto countFunction = [](uint32_t address)
    {
        return static_cast<uint8_t>(address % 0xFF);
    };
#endif // MEM_ACCESS_WIDTH_BIT == 16
    auto ret = FillMemoryArea(0, m_MemorySize, countFunction);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    return VerifyMemoryArea(0, m_MemorySize, countFunction);
}

/*
 * @brief								fills the whole SRAM with alternating 010101010....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillMemoryWithAlternatingZeroAndOne()
{
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t value = 0x5555;
#elif MEM_ACCESS_WIDTH_BIT == 8
    uint8_t value = 0x55;
#endif // MEM_ACCESS_WIDTH_BIT == 16

    return FillMemoryArea(0, m_MemorySize, value);
}

/*
 * @brief								fills the whole SRAM with alternating 101010101....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillMemoryWithAlternatingOneAndZero()
{
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t value = 0xAAAA;
#elif MEM_ACCESS_WIDTH_BIT == 8
    uint8_t value = 0xAA;
#endif // MEM_ACCESS_WIDTH_BIT == 16

    return FillMemoryArea(0, m_MemorySize, value);
}


/*
 * @brief							resets the counters for statistical analysis
 */
void MemoryController::InitCounter(){
    total_one = 0;
    total_zero = 0;
    flipped_one = 0;
    flipped_zero = 0;
}

/*
 * @brief							resets the custom start/end addresses and the custom start value
 * 									these variables are set in a write method
 */
void MemoryController::InitArguments(){
    start_adr = 0;
    end_adr = 0;
    start_value = 0;
}

/*static*/ bool MemoryController::IsInvalidAddress(uint32_t address, uint32_t size)
{
    return (address >= size) ?  true : false;
}

MEM_ERROR MemoryController::Initialize()
{
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Write8BitWord(uint32_t adr, uint8_t value)
{
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Read8BitWord(uint32_t adr, uint8_t *ret)
{
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Write16BitWord(uint32_t adr, uint16_t value)
{
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Read16BitWord(uint32_t adr, uint16_t *value)
{
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

