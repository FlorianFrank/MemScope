/**
  ******************************************************************************
  * File Name          : tud_seceng.h
  * Description        : This file contains the common defines of the methods
  ******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEMORY_CONTROL_H
#define __MEMORY_CONTROL_H

#include "SystemFiles/usbd_cdc_if.h"
#include "memory_defines.h"
#include "memory_error_handling.h"


typedef enum {
	FAILED = 0,
	PASSED = !FAILED
} TestStatus; // Typedef for correct read/write tests

#define STRING_BUFFER_SIZE			100							// buffer size for the global string buffer


extern char Rx_Buffer[100];
extern uint8_t write_mode;
extern int Transfer_cplt;
extern uint16_t len;
extern char STRING_BUFFER[STRING_BUFFER_SIZE];



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

/* see fmc.c
 * FMC_NORSRAM_BANK1 = 0x60000000
 * FMC_NORSRAM_BANK2 = 0x64000000
 * FMC_NORSRAM_BANK3 = 0x68000000
 * FMC_NORSRAM_BANK4 = 0x6C000000
 * We use Bank 1, Block 2, thus FMC_NE2 and 0x64000000
 * */
#define SRAM_BANK_ADDR		((uint32_t)0x64000000)

#define SRAM_BUFFER_SIZE	40960						// 5x8192 buffer size for the sram buffer
#define COMMAND_COUNT		15

// global string buffer

extern void receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize);
extern void receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize);


extern void showHelpUSB();

extern void executeCommandUSB();

// functions to access the SRAM
MEM_ERROR MemoryWrite8BitWord(uint32_t adr, uint8_t value);
MEM_ERROR MemoryRead8BitWord(uint32_t adr, uint8_t *ret);
MEM_ERROR MemoryWrite16BitWord(uint32_t adr, uint16_t value);
MEM_ERROR MemoryRead16BitWord(uint32_t adr, uint16_t *value);


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


// helper functions
void clearBuffer(uint8_t index);
void init_counter(void);

void init_arguments(void);
uint8_t get_space(char *rx_buffer);

#if MEM_ACCESS_IF==SPI
uint32_t WIP_Polling(uint32_t timeoutCycles);
MEM_ERROR Set_WriteEnable();
MEM_ERROR Reset_WriteEnable();
MEM_ERROR Set_WriteEnableLatch(bool checkRegister);

__unused MEM_ERROR Reset_WriteEnableLatch();
MEM_ERROR ReadStatusRegister(MemoryStatusRegister *statusRegister);
__unused MEM_ERROR WriteStatusRegister(MemoryStatusRegister *statusRegister, uint8_t value);
__unused MEM_ERROR EraseChip();
__unused MEM_ERROR ResumeFromPowerDown();
__unused MEM_ERROR SetUltraDeepPowerDown();
__unused MEM_ERROR SetSleepMode();

__unused __unused MEM_ERROR SetPowerDown();

#endif // MEM_ACCESS_IF==SPI

#endif /* __MEMORY_CONTROL_H */
