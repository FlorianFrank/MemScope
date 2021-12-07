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

/*
 * @brief								fills the whole SRAM with 0's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillWithZeros(uint8_t *buffer, uint32_t *buffLen){

    if(!buffer || !buffLen || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    write_mode = 0x1;
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0xFFFF;
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        MEM_ERROR ret = Write16BitWord(adr, 0x0);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        // test, if the written value equals the expected value
        // if something went wrong, break here and display an error message
        if(real_value != 0x0){
            state = FAILED;
            break;
        }
    }
#endif //

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0xFF;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			MEM_ERROR ret  = SRAM_Write_8b(adr, 0x0);
			if(ret != MEM_NO_ERROR)
				return ret;

			ret = MemoryRead8BitWord(adr, &real_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			// test, if the written value equals the expected value
			// if something went wrong, break here and display an error message
			if(real_value != 0x0){
				state = FAILED;
				break;
			}
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;
    // display return message
    if(state == PASSED){
        sprintf((char*)buffer, "'writeZeroAll' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeZeroAll' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    sprintf((char*)buffer, "\n\r~~~~~~~~~~~~~~~\n\r"); // FIXME
    *buffLen = strlen((char*)buffer);

    return retCode;
}

/*
 * @brief								fills the whole SRAM with 1's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillWithOnes(uint8_t *buffer, uint32_t *bufferLen){

    if(!buffer || !bufferLen || *bufferLen)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0x0;
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        MEM_ERROR ret = Write16BitWord(adr, 0xFFFF);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        // test, if the written value equals the expected value
        // if something went wrong, break here and display an error message
        if(real_value != 0xFFFF){
            state = FAILED;
            break;
        }
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			MEM_ERROR ret = SRAM_Write_8b(adr, 0xFF);
			if(ret != MEM_NO_ERROR)
				return ret;

			ret = MemoryRead8BitWord(adr, &real_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			//real_value = *(__IO uint8_t *) (MEMORY_BANK_ADDRESS + adr);
			// test, if the written value equals the expected value
			// if something went wrong, break here and display an error message
			if(real_value != 0xFF){
				state = FAILED;
				break;
			}
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;

    // display return message
    if(state == PASSED){
        sprintf((char*)buffer, "'writeOneAll' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeOneAll' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }

    sprintf((char*)buffer, "\n\r~~~~~~~~~~~~~~~\n\r");
    len = strlen((char*)buffer);
    *bufferLen = len;

    return retCode;
}

MEM_ERROR MemoryController::ReadArea(uint8_t *buffer, uint32_t *bufferLen){

    if(!buffer || !bufferLen || *bufferLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    total_one = 0;
    total_zero = 0;

#if MEM_ACCESS_WIDTH_BIT == 16
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        uint16_t real_value;
        MEM_ERROR ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        total_one += get_num_one_16b(real_value);
        total_zero += get_num_zero_16b(real_value);
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
				uint8_t real_value;
				MEM_ERROR ret = MemoryRead8BitWord(adr, &real_value);
				if(ret != MEM_NO_ERROR)
					return ret;

				total_one += get_num_one_8b(real_value);
				total_zero += get_num_zero_8b(real_value);
		}
#endif //MEM_ACCESS_WIDTH_BIT == 8

    // displays the total 1's
    sprintf((char*)buffer, "Total number of ones in SRAM:  %lu\r\n", (unsigned long)total_one);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)STRING_BUFFER, len);

    // displays the total 0's
    sprintf((char*)&buffer[len], "Total number of zeros in SRAM:  %lu\r\n", (unsigned long)total_zero);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)buffer, len);

#if MEM_ACCESS_WIDTH_BIT == 16
    // displays the total 0's
    sprintf((char*)&buffer[len], "Percent zeros:  %.16f%%\r\n", (float)total_zero / (MEM_SIZE_ADR * 16) * 100);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)buffer, len);

    // displays the total 0's
    sprintf((char*)&buffer[len], "Percent ones:  %.16f%%\r\n", (float)total_one / (MEM_SIZE_ADR * 16) * 100);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)buffer, len);
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    // displays the total 0's - (float)total_zero / (SRAM_SIZE * 8) * 100);
		sprintf((char*)&buffer[len], "Percent zeros:  %.8f%%\r\n", (float)total_zero / (MEM_SIZE_ADR * 8) * 100);
		len = strlen((char*)buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);

		// displays the total 0's - (float)total_one / (SRAM_SIZE * 8) * 100);
		sprintf((char*)&buffer[len], "Percent ones:  %.8f%%\r\n", (float)total_one / (MEM_SIZE_ADR * 8) * 100);
		len = strlen((char*)buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
#endif // MEM_ACCESS_WIDTH_BIT == 8
    *bufferLen = len;

    return MemoryErrorHandling::MEM_NO_ERROR;
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
        //send(huart, (uint8_t *)STRING_BUFFER, len);
    }else{
        // displays the probability that a 1 changes to a 0
        float p_one_to_zero = ((float)flipped_one / (float)total_one) * 100;
        sprintf((char*)buffer, "P(1->0) = %.16f%%\r\n", p_one_to_zero);
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)STRING_BUFFER, len);
    }

    if(total_zero == 0){
        sprintf((char*)&buffer[len], "P(0->1) = NaN\r\n");
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)STRING_BUFFER, len);
    }else{
        // displays the probability that a 0 changes to a 1
        float p_zero_to_one = ((float)flipped_zero / (float)total_zero) * 100;
        sprintf((char*)&buffer[len], "P(0->1) = %.16f%%\r\n", p_zero_to_one);
        len = strlen((char*)buffer);
        //send(huart, (uint8_t *)STRING_BUFFER, len);
    }
    float p_total_flip_probability = ((float)(flipped_one + flipped_zero) / (float)(total_one + total_zero)) * 100;

    // displays the total flip probability
    sprintf((char*)&buffer[len], "P(flip) = %.16f%%\r\n", p_total_flip_probability);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)STRING_BUFFER, len);

    // displays the total flip 1's
    sprintf((char*)&buffer[len], "Total number of flipped 1->0:  %lu\r\n", (unsigned long)flipped_one);
    len = strlen((char*)buffer);

    // displays the total 1's
    sprintf((char*)&buffer[len], "Total number of ones in expected value:  %lu\r\n", (unsigned long)total_one);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)STRING_BUFFER, len);

    // displays the total flip 0's
    sprintf((char*)&buffer[len], "Total number of flipped 0->1:  %lu\r\n", (unsigned long)flipped_zero);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)STRING_BUFFER, len);

    // displays the total 0's
    sprintf((char*)&buffer[len], "Total number of zeros in expected value:  %lu\r\n", (unsigned long)total_zero);
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)STRING_BUFFER, len);

    // displays the total flipped bits
    sprintf((char*)&buffer[len], "Total number of flipped bits:  %lu\r\n\n", (unsigned long)(flipped_one + flipped_zero));
    len = strlen((char*)buffer);
    //send(huart, (uint8_t *)STRING_BUFFER, len);
    *buffLen = len;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*
 * @brief								fills the first address SRAM with the argument and increment it by one
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the start value to count up
 */
MEM_ERROR MemoryController::FillMemoryWithAscendingValues(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments){
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    start_value = (uint16_t)arguments[0];
    uint16_t real_value = 0x0;
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        MEM_ERROR ret = Write16BitWord(adr, start_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        if(real_value != start_value){
            state = FAILED;
            break;
        }
        start_value++;
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    start_value = (uint8_t)args[0];
		uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			MEM_ERROR ret = SRAM_Write_8b(adr, start_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			ret = MemoryRead8BitWord(adr, &real_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			if(real_value != start_value){
				state = FAILED;
				break;
			}
			start_value++;
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;

    if(state == PASSED){
        sprintf((char*)buffer, "'writeValueAsc' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeValueAsc' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    len = strlen((char*)buffer);
    *buffLen = len;
    return retCode;
}

/*
 * @brief								fills the whole SRAM with alternating 010101010....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::WriteAlternatingZeroAndOne(uint8_t *buffer, uint32_t *bufferLen){
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0x0;
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        MEM_ERROR ret = Write16BitWord(adr, 0x5555);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        if(real_value != 0x5555){
            state = FAILED;
            break;
        }
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			MEM_ERROR ret = SRAM_Write_8b(adr, 0x55);
			if(ret != MEM_NO_ERROR)
				return ret;

			ret = MemoryRead8BitWord(adr, &real_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			if(real_value != 0x55){
				state = FAILED;
				break;
			}
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;

    if(state == PASSED){
        sprintf((char*)buffer, "'writeAlternateZeroOne' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeAlternateZeroOne' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    len = strlen((char*)buffer);

    return retCode;
}

/*
 * @brief								fills the whole SRAM with alternating 101010101....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::WriteAlternatingOneAndZero(uint8_t *buffer, uint32_t *bufferLen){
    if(!buffer || !bufferLen || *bufferLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0x0;
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        MEM_ERROR ret = Write16BitWord(adr, 0xAAAA);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        if(real_value != 0xAAAA){
            state = FAILED;
            break;
        }
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			MEM_ERROR ret = SRAM_Write_8b(adr, 0xAA);
			if(ret != MEM_NO_ERROR)
				return ret;

			ret = MemoryRead8BitWord(adr, &real_value);
			if(ret != MEM_NO_ERROR)
				return ret;

			if(real_value != 0xAA){
				state = FAILED;
				break;
			}
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;
    if(state == PASSED){
        sprintf((char*)buffer, "'writeAlternateOneZero' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeAlternateOneZero' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    len = strlen((char*)buffer);
    *bufferLen = len;

    return retCode;
}

/*
 * @brief								writes the value (second entry in parameter m_arguments) to the
 * 										specified address (first entry in parameter m_arguments)
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			first element is the address, second element is the value
 */
MEM_ERROR MemoryController::WriteSingleValue(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments){
    if(!buffer || !buffLen || !arguments || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = FAILED;

    // set the variables for checking and determining the statistical values
    start_adr = arguments[0];
    end_adr = start_adr + 1;


#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0;
    start_value = (uint16_t)arguments[1];
    MEM_ERROR ret = Write16BitWord(start_adr, start_value);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = Read16BitWord(start_adr, &real_value);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    if(real_value == start_value){
        state = PASSED;
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0;
		start_value = (uint8_t)args[1];

		MEM_ERROR ret = SRAM_Write_8b(start_adr, start_value);
		if(ret != MEM_NO_ERROR)
			return ret;

		ret = MemoryRead8BitWord(start_adr, &real_value);
		if(ret != MEM_NO_ERROR)
			return ret;

		if(real_value == start_value){
			state = PASSED;
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;

    if(state == PASSED){
        sprintf((char*)buffer, "'writeSRAM' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeSRAM' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    len = strlen((char*)buffer);
    *buffLen = len;

    return retCode;
}

/*
 * @brief								writes the value (third entry in parameter m_arguments) to the
 * 										specified addresses (from first entry to second entry
 * 										in parameter m_arguments)
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			first element is the start address, second element is the end address
 * 										third element is the value
 */
MEM_ERROR MemoryController::WriteAddressRange(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments){

    if(!buffer || !buffLen || !arguments || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    TestStatus state = PASSED;


    // set the variables for checking and determining the statistical values
    // correct the end_adr for the loop (< not <=)
    start_adr = arguments[0];
    end_adr = arguments[1] + 1;

#if MEM_ACCESS_WIDTH_BIT == 16
    start_value = (uint16_t)arguments[2];
    uint16_t real_value;
    if(start_adr < MEM_SIZE_ADR && end_adr <= MEM_SIZE_ADR){
        start_adr = arguments[0];
        end_adr = arguments[1] + 1;
        for(uint32_t adr = start_adr; adr < end_adr; adr++){
            MEM_ERROR ret = Write16BitWord(adr, start_value);
            if(ret != MemoryErrorHandling::MEM_NO_ERROR)
                return ret;

            ret = Read16BitWord(adr, &real_value);
            if(ret != MemoryErrorHandling::MEM_NO_ERROR)
                return ret;

            if(real_value != start_value){
                state = FAILED;
                break;
            }
        }
    }else{
        state = FAILED;
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    start_value = (uint8_t)args[2];
		uint8_t real_value;
		if(start_adr < MEM_SIZE_ADR && end_adr <= MEM_SIZE_ADR){
			start_adr = args[0];
			end_adr = args[1] + 1;
			for(uint32_t adr = start_adr; adr < end_adr; adr++){
				MEM_ERROR ret = SRAM_Write_8b(adr, start_value);
				if(ret != MEM_NO_ERROR)
					return ret;

				ret = MemoryRead8BitWord(adr, &real_value );
				if(ret != MEM_NO_ERROR)
					return ret;

				//real_value = *(__IO uint8_t *) (MEMORY_BANK_ADDRESS + adr);
				if(real_value != start_value){
					state = FAILED;
					break;
				}
			}
		}else{
			state = FAILED;
		}
#endif // #if MEM_ACCESS_WIDTH_BIT == 8

    MEM_ERROR retCode = MemoryErrorHandling::MEM_NO_ERROR;

    if(state == PASSED){
        sprintf((char*)buffer, "'writeSRAMRange' was successful\r\n\n");
    }else{
        sprintf((char*)buffer, "'writeSRAMRange' failed\r\n\n");
        retCode = MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    len = strlen((char*)buffer);
    *buffLen = len;

    return retCode;
}

/*
 * @brief								reads the whole SRAM and print it as hexadecimal values
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::ReadWholeMemory(uint8_t *buffer, uint32_t *buffLen){

    srambp = SRAM_BUFFER;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value;
    uint16_t counter = 0;
    for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
        MEM_ERROR ret = Read16BitWord(adr, &real_value);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;

        srambp += sprintf(srambp, "%04X\t", real_value);// 0xff 0xffff 0xffffffff
        counter++;
        if(counter == 8192){
            len = strlen(SRAM_BUFFER);
            counter = 0;
            srambp = SRAM_BUFFER;
        }
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value;
		uint16_t counter = 0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			MEM_ERROR ret = MemoryRead8BitWord(adr, &real_value );
			if(ret != MEM_NO_ERROR)
				return ret;
			//real_value = *(__IO uint8_t *) (MEMORY_BANK_ADDRESS + adr);
			srambp += sprintf(srambp, "%02X\t", real_value);
			counter++;
			if(counter == 1024){ //8192//512//1024
				len = strlen(SRAM_BUFFER);
				counter = 0;
				srambp = SRAM_BUFFER;
			}
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*
 * @brief								prints the address and the value at address (first entry in
 * 										parameter m_arguments)
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the address to read from
 */
MEM_ERROR MemoryController::GetValueAndAddress(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args)
{
    if (!buffer || !buffLen || !args || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0;
    MEM_ERROR ret = Read16BitWord(args[0], &real_value);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    sprintf((char*)buffer, "Address: %lu\tValue: %#06X\n\n\r", (unsigned long)args[0], real_value);
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0;
    MEM_ERROR ret = MemoryRead8BitWord(args[0], &real_value);
    if (ret != MEM_NO_ERROR)
        return ret;
    sprintf((char *) buffer, "Address: %lu\tValue: %#03X\n\n\r", (unsigned long) args[0], real_value);
#endif // MEM_ACCESS_WIDTH_BIT == 8

    len = strlen((char *) buffer);
    *buffLen = len;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*
 * @brief								checks if the expected value (second entry in parameter m_arguments)
 * 										equals the value stored at address specified in first entry in
 * 										parameter m_arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the address to read from, the value to compare with
 */
MEM_ERROR MemoryController::CheckExpectedValueAtAddress(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args){
    if(!buffer || !buffLen || !args || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t expected_value = (uint16_t)args[1];
    uint16_t real_value = 0;
    MEM_ERROR ret = Read16BitWord(args[0], &real_value);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    if(real_value != expected_value){
        state = FAILED;
    }
    if(state == PASSED){
        //sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
        sprintf((char*)buffer, "TRUE\n\n\r");
    }else{
        sprintf((char*)buffer, "%#06X != %#06X\n\rFALSE\n\n\r", real_value, expected_value);
        return MemoryErrorHandling::MEM_UNEXPECTED_VALUE;
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t expected_value = (uint8_t)args[0];
		uint8_t real_value = 0;
		MEM_ERROR ret = MemoryRead8BitWord(args[0], &real_value);
		if(ret != MEM_NO_ERROR)
			return ret;
		if(real_value != expected_value){
			state = FAILED;
		}
		if(state == PASSED){
			//sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
			sprintf((char*)buffer, "TRUE\n\n\r");
		}else{
			sprintf((char*)buffer, "%#03X != %#03X\n\rFALSE\n\n\r", real_value, expected_value);
			return MEM_UNEXPECTED_VALUE;
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    len = strlen((char*)buffer);
    *buffLen = len;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*
 * @brief								checks if the expected value (third entry in parameter m_arguments)
 * 										equals the value stored at address specified in first entry in
 * 										parameter m_arguments to address specified in second entry in
 * 										parameter m_arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the start address to read from, the end address to read from
 * 										the value to compare with
 */
MEM_ERROR MemoryController::CheckExpectedValueAtAddressRange(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args){
    uint32_t start_local = args[0];
    uint32_t end_local = args[1];
    TestStatus state = PASSED;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t expected_value_local = (uint16_t)args[2];
    uint16_t real_value_local = 0;
    if(start_local <= end_local){
        if(start_local < MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
            for(uint32_t adr = start_local; adr < end_local; adr++){
                MEM_ERROR ret = Read16BitWord(adr, &real_value_local);
                if(ret != MemoryErrorHandling::MEM_NO_ERROR)
                    return ret;

                if(real_value_local != expected_value_local){
                    state = FAILED;
                    sprintf((char*)buffer, "Address: %lu\t%#06X != %#06X\n\r", (unsigned long)adr, real_value_local, expected_value_local);
                    len = strlen((char*)buffer);
                }
            }
        }else{
            state = FAILED;
        }
    }else{
        state = FAILED;
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t expected_value_local = (uint8_t)args[2];
		uint8_t real_value_local = 0;
		if(start_local <= end_local){
			if(start_local < MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
				for(uint32_t adr = start_local; adr < end_local; adr++){
					MEM_ERROR ret = MemoryRead8BitWord(adr, &real_value_local);
					if(ret != MEM_NO_ERROR)
						return ret;
					if(real_value_local != expected_value_local){
						state = FAILED;
						sprintf((char*)buffer, "Address: %lu\t%#03X != %#06X\n\r", (unsigned long)adr, real_value_local, expected_value_local);
						len = strlen((char*)buffer);
					}
				}
			}else{
				state = FAILED;
			}
		}else{
			state = FAILED;
		}
#endif // MEM_ACCESS_WIDTH_BIT == 8

    if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
        //sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
        sprintf((char*)&buffer[len], "TRUE\n\n\r");
    }else{
        sprintf((char*)&buffer[len], "FALSE\n\n\r");
        return MemoryErrorHandling::MEM_UNEXPECTED_VALUE;
    }
    len = strlen((char*)buffer);
    *buffLen = len;

    return MemoryErrorHandling::MEM_NO_ERROR;
}


/*
 * @brief								checks if the expected value (third entry in parameter m_arguments)
 * 										equals the value stored at address specified in first entry in
 * 										parameter m_arguments to address specified in second entry in
 * 										parameter m_arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the start address to read from, the end address to read from
 * 										the value to compare with
 */
MEM_ERROR MemoryController::CheckReadWriteStatus(uint8_t *buffer, uint32_t *buffLen){
    if(!buffer || !buffLen || *buffLen == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    // if no write access was performed => warning
    if(write_mode == 0xFF){
        sprintf((char*)buffer, "Caution. No write access was performed. Therefore we have no values to compare with...\r\nExit\r\n\n");
        len = strlen((char*)buffer);
        *buffLen = len;
        return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }

    TestStatus state = PASSED;
    uint32_t start_local = 0;
    uint32_t end_local = MEM_SIZE_ADR;

#if MEM_ACCESS_WIDTH_BIT == 16
    uint16_t real_value = 0;
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
            start_local = start_value;
            end_local = end_adr;
            break;
    }

    len = 0;
    if(start_local <= MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
        for(uint32_t adr = start_local; adr < end_local; adr++){
            MEM_ERROR ret = Read16BitWord(adr, &real_value);
            if(ret != MemoryErrorHandling::MEM_NO_ERROR)
                return ret;
            if(real_value != expected_value){
                state = FAILED;
                sprintf((char*)&buffer[len], "Address: %lu\t%#06X != %#06X\n\r", (unsigned long)adr, real_value, expected_value);
                len = strlen((char*)buffer);
            }
            if(write_mode == 3){
                expected_value++;
            }
        }
    }else{
        state = FAILED;
    }
#endif // MEM_ACCESS_WIDTH_BIT == 16

#if MEM_ACCESS_WIDTH_BIT == 8
    uint8_t real_value = 0;
		uint8_t expected_value = 0;
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
			end_local = end_adr;
			break;
		case WRITE_ADDRESS_RANGE:
			// writeSRAMRange
			expected_value = start_value;
			start_local = start_value;
			end_local = end_adr;
			break;
		default:
		    return MEM_INVALID_COMMAND;
		}

		if(start_local <= MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
			for(uint32_t adr = start_local; adr < end_local; adr++){
				MEM_ERROR ret = MemoryRead8BitWord(adr, &real_value);
				if(ret != MEM_NO_ERROR)
					return ret;
				if(real_value != expected_value){
					state = FAILED;
					sprintf((char*)&buffer[len], "Address: %lu\t%#06X != %#06X\n\r", (unsigned long)adr, real_value, expected_value);
					len = strlen((char*)buffer);
					//send(huart, (uint8_t *)STRING_BUFFER, len);
				}
				if(m_WriteMode == 3){
					expected_value++;
				}
			}
		}else{
			state = FAILED;
		}
		*buffLen = len;
#endif // MEM_ACCESS_WIDTH_BIT == 8


    if(state == PASSED){
        //sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
        sprintf((char*)&buffer[len], "TRUE\n\n\r");
    }else{
        sprintf((char*)&buffer[len], "FALSE\n\n\r");
        return MemoryErrorHandling::MEM_UNEXPECTED_VALUE;
    }
    len = strlen((char*)buffer);
    *buffLen = len;
    return MemoryErrorHandling::MEM_NO_ERROR;
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

