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

class SPIWrapper;

#define SRAM_BUFFER_SIZE	40960						// 5x8192 buffer size for the sram buffer


enum
{
    ReRAM_WREN      = (uint8_t) 0x06,       // Set Write Enable Latch
    ReRAM_WRDI      = (uint8_t) 0x04,       // Reset Write Enable Latch
    ReRAM_RDSR      = (uint8_t) 0x05,       // Read Status Register
    ReRAM_WRSR      = (uint8_t) 0x01,       // Write Status Register
    ReRAM_READ      = (uint8_t) 0x03,       // Read Memory Code
    ReRAM_WRITE     = (uint8_t) 0x02,       // Write Memory Code
    ReRAM_PD        = (uint8_t) 0x09,       // Chip Erase
    ReRAM_UDPD      = (uint8_t) 0x79,       // Ultra Deep Power Down
    ReRAM_RES       = (uint8_t) 0xAB,       // Resume from Power Down
    ReRAM_RDID      = (uint8_t) 0b10011111, // Read Device ID
    ReRAM_SLEEP     = (uint8_t) 0b10111001, // Sleep Mode
    ReRam_PowerDown = (uint8_t) 0xB9        //
} typedef SPI_Commands;

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class MemoryController
{
public:

    MemoryController(InterfaceWrappers &interfaceWrapper);
    ~MemoryController();

    typedef enum {
        FAILED = 0,
        PASSED = !FAILED
    } TestStatus; // Typedef for correct read/write Tests


//#if MEM_ACCESS_IF==SPI
    struct
    {
        uint8_t wp_enable_Pin;
        uint8_t auto_power_down_enable;
        uint8_t low_power_standby_enable;
        uint8_t block_protection_bits;
        uint8_t write_enable_bit;
        uint8_t write_in_progress_bit;

    } typedef MemoryStatusRegister;


// functions to access the SRAM
    MEM_ERROR MemoryWrite8BitWord(uint32_t adr, uint8_t value);
    MEM_ERROR MemoryRead8BitWord(uint32_t adr, uint8_t *ret) const;
    MEM_ERROR MemoryWrite16BitWord(uint32_t adr, uint16_t value);
    MEM_ERROR MemoryRead16BitWord(uint32_t adr, uint16_t *value) const;


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

    uint8_t get_space(char *rx_buffer);


private:
    // helper functions
    void init_counter(void);

    void init_arguments(void);

    void USBCDCRXCallback(uint8_t *Buf, uint32_t Len);


#if MEM_ACCESS_IF==SPI
    uint32_t WIP_Polling(uint32_t timeoutCycles);
    MEM_ERROR Set_WriteEnable();
    MEM_ERROR Reset_WriteEnable();
    MEM_ERROR Set_WriteEnableLatch(bool checkRegister);

    MEM_ERROR Reset_WriteEnableLatch();
    MEM_ERROR ReadStatusRegister(MemoryStatusRegister *statusRegister);
    MEM_ERROR WriteStatusRegister(MemoryStatusRegister *statusRegister, uint8_t value);
    MEM_ERROR EraseChip();
    MEM_ERROR ResumeFromPowerDown();
    MEM_ERROR SetUltraDeepPowerDown();
    MEM_ERROR SetSleepMode();

    MEM_ERROR SetPowerDown();

    MEM_ERROR SendSPICommand(SPI_Commands spiCMD, uint8_t *retValue, bool response);
    MemoryStatusRegister ParseStatusRegister(uint8_t statusRegister);
    void PrintStatusRegister(MemoryStatusRegister reg);
#endif // SPI

    static inline bool isInvalidAddress(uint32_t address);


    char Rx_Buffer[100];
    uint8_t write_mode;
    int Transfer_cplt;
    uint16_t len;
    char STRING_BUFFER[STRING_BUFFER_SIZE];
    TimeMeasurement m_timeMeasurement;

    uint32_t m_MMIOStartAddress;
    SPIWrapper* m_SPIWrapper;
    InterfaceWrappers m_InterfaceWrapper;

    // commands
    uint16_t start_value = 0x0; // start value for ascending writing
    uint32_t start_adr = 0x0;	// start address for writeSRAMRange
    uint32_t end_adr = 0x0;		// end address for writeSRAMRange

    // global string buffer
    //char STRING_BUFFER[STRING_BUFFER_SIZE];
    char SRAM_BUFFER[SRAM_BUFFER_SIZE];
    char *srambp;

    // probability counter
// 1 MB RAM => 1 million 1's and 0's possible => uint32_t
    uint32_t total_one;
    uint32_t total_zero;
    uint32_t flipped_one;
    uint32_t flipped_zero;

};



#endif /* __MEMORY_CONTROL_H */


/**
  ******************************************************************************
  * File Name          : tud_seceng.h
  * Description        : This file contains the common defines of the methods
  ******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
/*#ifndef __MEMORY_CONTROL_H

#include "SystemFiles/usbd_cdc_if.h"
#include "memory_defines.h"
#include "memory_error_handling.h"






#if MEM_ACCESS_IF==SPI
struct
{
    uint8_t wp_enable_Pin;
    uint8_t auto_power_down_enable;
    uint8_t low_power_standby_enable;
    uint8_t block_protection_bits;
    uint8_t write_enable_bit;
    uint8_t write_in_progress_bit;

} typedef MemoryStatusRegister;
#endif // MEM_ACCESS_IF==SPI


// global string buffer



*/

