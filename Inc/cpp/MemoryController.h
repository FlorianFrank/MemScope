//
// Created by florianfrank on 03.12.21.
//

//#include <stm32f4xx_hal_uart.h>
//#include "cpp/UARTWrapper.h"
//#include "cpp/USBWrapper.h"


#ifndef MEMORY_TESTING_FW_MEMORYCONTROLLER_H
#define MEMORY_TESTING_FW_MEMORYCONTROLLER_H

#include "cpp/MemoryErrorHandling.h"
#include "TimeMeasurement.h"
#include "io_pin_defines.h"
#include "InterfaceWrappers.h"

/* see fmc.c
 * FMC_NORSRAM_BANK1 = 0x60000000
 * FMC_NORSRAM_BANK2 = 0x64000000
 * FMC_NORSRAM_BANK3 = 0x68000000
 * FMC_NORSRAM_BANK4 = 0x6C000000
 * We use Bank 1, Block 2, thus FMC_NE2 and 0x64000000
 * */

#define COMMAND_COUNT		15

// Define IO to empty when not using STM32
#ifndef __IO
#define __IO volatile
#endif // !__IO

class SPIWrapper;

#define SRAM_BUFFER_SIZE	40960						// 5x8192 buffer size for the sram buffer

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class MemoryController
{
public:

    explicit MemoryController(InterfaceWrappers &interfaceWrapper);
    ~MemoryController();

    typedef enum {
        FAILED = 0,
        PASSED = !FAILED
    } TestStatus; // Typedef for correct read/write Tests


//#if MEM_ACCESS_IF==SPI


// functions to access the SRAM
    virtual MEM_ERROR MemoryWrite8BitWord(uint32_t adr, uint8_t value) = 0;
    virtual MEM_ERROR MemoryRead8BitWord(uint32_t adr, uint8_t *ret) const = 0;
    virtual MEM_ERROR MemoryWrite16BitWord(uint32_t adr, uint16_t value) = 0;
    virtual MEM_ERROR MemoryRead16BitWord(uint32_t adr, uint16_t *value) const = 0;


// user functions
    MEM_ERROR MemoryFillWithZeros(uint8_t *buffer, uint32_t *buffLen);
    MEM_ERROR MemoryFillWithOnes(uint8_t *buffer, uint32_t *bufferLen);
    MEM_ERROR MemoryReadArea(uint8_t *buffer, uint32_t *bufferLen);

    MEM_ERROR MemoryGetProbabilityOfFlippedOnesAndZeros(uint8_t *buffer, uint32_t *buffLen);
    MEM_ERROR MemoryFillMemoryWithAscendingValues(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments);
    MEM_ERROR MemoryWriteAlternatingZeroAndOne(uint8_t *buffer, uint32_t *bufferLen);
    MEM_ERROR MemoryWriteAlternatingOneAndZero(uint8_t *buffer, uint32_t *bufferLen);
    MEM_ERROR MemoryWriteSingleValue(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments);
    MEM_ERROR MemoryWriteAddressRange(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments);
    MEM_ERROR MemoryReadWholeMemory(uint8_t *buffer, uint32_t *buffLen);
    MEM_ERROR SRAM_Get_Address(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args);
    MEM_ERROR SRAM_Check_Address(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args);
    MEM_ERROR SRAM_Check_Address_Range(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args);
    MEM_ERROR SRAM_Check_Read_Write_Status(uint8_t *buffer, uint32_t *buffLen);
    MEM_ERROR SRAM_Measure_WIP_Polling();

protected:
    static bool isInvalidAddress(uint32_t address);


    // helper functions
    void init_counter();

    void init_arguments();

    char Rx_Buffer[100];
    uint8_t write_mode;
    int Transfer_cplt;
    uint16_t len;
    char STRING_BUFFER[STRING_BUFFER_SIZE];
    TimeMeasurement m_timeMeasurement;

    uint32_t m_MMIOStartAddress;
    SPIWrapper* m_SPIWrapper;
    InterfaceWrappers m_InterfaceWrapper{};

    // commands
    uint16_t start_value = 0x0; // start value for ascending writing
    uint32_t start_adr = 0x0;	// start address for writeSRAMRange
    uint32_t end_adr = 0x0;		// end address for writeSRAMRange

    // global string buffer
    //char STRING_BUFFER[STRING_BUFFER_SIZE];
    char SRAM_BUFFER[SRAM_BUFFER_SIZE]{};
    char *srambp{};

    // probability counter
// 1 MB RAM => 1 million 1's and 0's possible => uint32_t
    uint32_t total_one;
    uint32_t total_zero;
    uint32_t flipped_one;
    uint32_t flipped_zero;

};

#endif /* __MEMORY_CONTROL_H */
