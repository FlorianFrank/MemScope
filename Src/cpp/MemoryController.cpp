/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/MemoryController.h"
#include <CommandDefines.h>

#include "io_pin_defines.h"
#include <memory_defines.h>
extern "C" {
#include "metrics.h"
}
#include <cstring> // strstr, memset
#include <cstdio> // sprintf


#include "cpp/InterfaceWrappers/SPIWrapper.h"


using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;


MemoryController::MemoryController(InterfaceWrappers *interfaceWrapper) : write_mode(0xFF), len(0), STRING_BUFFER(""),
                                                                          m_MMIOStartAddress(MEMORY_BANK_ADDRESS),
                                                                          srambp(nullptr), total_one(0), total_zero(0),
                                                                          flipped_one(0), flipped_zero(0)
{
    m_InterfaceWrapper = interfaceWrapper;
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

    return FillMemoryArea(0, MEM_SIZE_ADR, value);
}


MEM_ERROR MemoryController::FillWithZerosAndVerifyRead()
{
#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t value = 0x0000;
#elif MEM_ACCESS_WIDTH_BIT == 8
    uint8_t value = 0x00;
#endif // MEM_ACCESS_WIDTH_BIT == 16
    auto ret = FillMemoryArea(0, MEM_SIZE_ADR, value);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    return VerifyMemoryArea(0, MEM_SIZE_ADR, value);
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

    return FillMemoryArea(0, MEM_SIZE_ADR, value);
}


/*
 * @brief								gets the probability of flipped 0's and 1's for the written values
 * 										displays an error message and exits, if no write operation was performed
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::GetProbabilityOfFlippedOnesAndZeros(uint8_t *buffer, uint32_t *buffLen){

    if(!buffer || !buffLen || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // if no write access was performed => warning
    if (write_mode == 0xFF) {
        sprintf((char*) buffer,
                "Caution. No write operation was performed. Therefore we have no values to compare with...\r\nExit\r\n\n");
        len = strlen((char*)buffer);
        return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }

    // reset the counter for statistical analysis
    InitCounter();
    uint32_t start_local = 0;
    uint32_t end_local = 0;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t expected_value = 0;

    // switch between the different command modes to set the correct values to compare with
    switch(write_mode){
        case FILL_WITH_ZEROS:
            // writeZeroAll
            expected_value = 0x0;
            break;
        case FILL_WITH_ONES:
            // writeOneAll
            expected_value = 0xFFFF;
            break;
        case WRITE_ASCENDING:
            // writeValueAsc
            expected_value = start_value;
            break;
        case WRITE_ALTERNATE_ZERO_ONE:
            // writeAlternateZeroOne
            expected_value = 0x5555;
            break;
        case WRITE_ALTERNATE_ONE_ZERO:
            // writeAlternateOneZero
            expected_value = 0xAAAA;
            break;
        case WRITE_ADDRESS:
            // writeSRAM
            expected_value = start_value;
            start_local = start_adr;
            end_local = end_adr;
            break;
        case WRITE_ADDRESS_RANGE:
            // writeSRAMRange
            expected_value = start_value;
            start_local = start_adr;
            end_local = end_adr;
            break;
    }
    //expected_value = 0x00FF;
    for(uint32_t adr = start_local; adr < end_local; adr++){
        uint16_t real_value;
        MEM_ERROR ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        flipped_one += flipped_one_16b(expected_value, real_value);
        flipped_zero += flipped_zero_16b(expected_value, real_value);
        total_one += get_num_one_16b(expected_value);
        total_zero += get_num_zero_16b(expected_value);
        // if 'writeValueAsc' increment the expected_value
        if(write_mode == 3){
            expected_value++;
        }
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t expected_value;

		// switch between the different command modes to set the correct values to compare with
		switch(m_WriteMode){
		case FILL_WITH_ZEROS:
			// writeZeroAll
			expected_value = 0x0;
			break;
		case FILL_WITH_ONES:
			// writeOneAll
			expected_value = 0xFF;
			break;
		case WRITE_ASCENDING:
			// writeValueAsc
			expected_value = start_value;
			break;
		case WRITE_ALTERNATE_ZERO_ONE:
			// writeAlternateZeroOne
			expected_value = 0x55;
			break;
		case WRITE_ALTERNATE_ONE_ZERO:
			// writeAlternateOneZero
			expected_value = 0xAA;
			break;
		case WRITE_ADDRESS:
			// writeSRAM
			expected_value = start_value;
			start_local = start_adr;
			end_local = start_adr + 1; // TODO
			break;
		case WRITE_ADDRESS_RANGE:
			// writeSRAMRange
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		default:
		    return MEM_INVALID_COMMAND;
		}
		//expected_value = 0x0F;
		for(uint32_t adr = start_local; adr < end_local; adr++){
			uint8_t real_value;
			MEM_ERROR ret = MemoryRead8BitWord(adr, &real_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			flipped_one += flipped_one_8b(expected_value, real_value);
			flipped_zero += flipped_zero_8b(expected_value, real_value);
			total_one += get_num_one_8b(expected_value);
			total_zero += get_num_zero_8b(expected_value);
			// if 'writeValueAsc' increment the expected_value
			if(m_WriteMode == 3){
				expected_value++;
			}
		}
#endif // #if MEM_ACCESS_WIDTH_BIT == 8

    // prevent division by zero
    if(total_one == 0){
        sprintf((char*)buffer, "P(1->0) = NaN\r\n");
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)m_SendBuffer, len);
    }else{
        // displays the probability that a 1 changes to a 0
        float p_one_to_zero = ((float)flipped_one / (float)total_one) * 100;
        sprintf((char*)buffer, "P(1->0) = %.16f%%\r\n", p_one_to_zero);
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)m_SendBuffer, len);
    }

    if(total_zero == 0){
        sprintf((char*)&buffer[len], "P(0->1) = NaN\r\n");
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)m_SendBuffer, len);
    }else{
        // displays the probability that a 0 changes to a 1
        float p_zero_to_one = ((float)flipped_zero / (float)total_zero) * 100;
        sprintf((char*)&buffer[len], "P(0->1) = %.16f%%\r\n", p_zero_to_one);
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)m_SendBuffer, len);
    }
    float p_total_flip_probability = ((float)(flipped_one + flipped_zero) / (float)(total_one + total_zero)) * 100;

    // displays the total flip probability
    sprintf((char*)&buffer[len], "P(flip) = %.16f%%\r\n", p_total_flip_probability);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)m_SendBuffer, len);

    // displays the total flip 1's
    sprintf((char*)&buffer[len], "Total number of flipped 1->0:  %lu\r\n", (unsigned long)flipped_one);
    len = strlen((char*)buffer);

    // displays the total 1's
    sprintf((char*)&buffer[len], "Total number of ones in expected value:  %lu\r\n", (unsigned long)total_one);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)m_SendBuffer, len);

    // displays the total flip 0's
    sprintf((char*)&buffer[len], "Total number of flipped 0->1:  %lu\r\n", (unsigned long)flipped_zero);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)m_SendBuffer, len);

    // displays the total 0's
    sprintf((char*)&buffer[len], "Total number of zeros in expected value:  %lu\r\n", (unsigned long)total_zero);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)m_SendBuffer, len);

    // displays the total flipped bits
    sprintf((char*)&buffer[len], "Total number of flipped bits:  %lu\r\n\n", (unsigned long)(flipped_one + flipped_zero));
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)m_SendBuffer, len);
    *buffLen = len;

    return MemoryErrorHandling::MEM_NO_ERROR;
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
    auto ret = FillMemoryArea(0, MEM_SIZE_ADR, countFunction);
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
    auto ret = FillMemoryArea(0, MEM_SIZE_ADR, countFunction);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    return VerifyMemoryArea(0, MEM_SIZE_ADR, countFunction);
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

    return FillMemoryArea(0, MEM_SIZE_ADR, value);
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

    return FillMemoryArea(0, MEM_SIZE_ADR, value);
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

/*static*/ bool MemoryController::IsInvalidAddress(uint32_t address)
{
    return (address >= MEM_SIZE_ADR) ?  true : false;
}

