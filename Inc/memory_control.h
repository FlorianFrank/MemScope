/**
  ******************************************************************************
  * File Name          : tud_seceng.h
  * Description        : This file contains the common defines of the methods
  ******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TUD_SECENG_H
#define __TUD_SECENG_H

#include "usbd_cdc_if.h"
#include "memory_defines.h"
#include "memory_error_handling.h"


typedef enum {
	FAILED = 0,
	PASSED = !FAILED
} TestStatus; // Typedef for correct read/write tests


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




// private defines
//#define SRAM_SIZE			((uint32_t)0x200000) 		// ‭2097152‬ bytes for MR4A08BYS35 has 2^18 address indices
//#define SRAM_SIZE			((uint32_t)0x80000) 		// Cypress CY62157EV30 has 2^19 address indices


/* see fmc.c
 * FMC_NORSRAM_BANK1 = 0x60000000
 * FMC_NORSRAM_BANK2 = 0x64000000
 * FMC_NORSRAM_BANK3 = 0x68000000
 * FMC_NORSRAM_BANK4 = 0x6C000000
 * We use Bank 1, Block 2, thus FMC_NE2 and 0x64000000
 * */
#define SRAM_BANK_ADDR		((uint32_t)0x64000000)

#define BUFFER_SIZE			100							// buffer size for the global string buffer
#define SRAM_BUFFER_SIZE	40960						// 5x8192 buffer size for the sram buffer
#define COMMAND_COUNT		15



// global string buffer
char STRING_BUFFER[BUFFER_SIZE];
char SRAM_BUFFER[SRAM_BUFFER_SIZE];
char *srambp;

uint16_t len;
uint16_t old_len;

// receive buffer
uint8_t Rx_Index;
char Rx_Data[2];
char Rx_Buffer[100];
char Transfer_cplt;





// probability counter
// 1 MB RAM => 1 million 1's and 0's possible => uint32_t
uint32_t total_one;
uint32_t total_zero;
uint32_t flipped_one;
uint32_t flipped_zero;


extern void receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize);
extern void receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize);


extern void showHelpUSB();

extern void executeCommandUSB();

// functions to access the SRAM
MEM_ERROR SRAM_Write_8b(const uint32_t adr, uint8_t value);
MEM_ERROR SRAM_Read_8b(const uint32_t adr, uint8_t *ret);
MEM_ERROR SRAM_Write_16b(uint32_t adr, uint16_t value);
MEM_ERROR SRAM_Read_16b(uint32_t adr, uint16_t *value);


// user functions
MEM_ERROR SRAM_Fill_With_Zeros(uint8_t *buffer, uint32_t *buffLen);
MEM_ERROR SRAM_Fill_With_Ones(uint8_t *buffer, uint32_t *bufferLen);
MEM_ERROR SRAM_Get_Values(uint8_t *buffer, uint32_t *bufferLen);

MEM_ERROR SRAM_Get_Performance_Measures(uint8_t *buffer, uint32_t *buffLen);
MEM_ERROR SRAM_Write_Ascending(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments);
MEM_ERROR SRAM_Write_Alternate_Zero_One(uint8_t *buffer, uint32_t *bufferLen);
MEM_ERROR SRAM_Write_Alternate_One_Zero(uint8_t *buffer, uint32_t *bufferLen);
MEM_ERROR SRAM_Write_Address(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments);
MEM_ERROR SRAM_Write_Address_Range(uint8_t *buffer, uint32_t *buffLen, const uint32_t *arguments);
MEM_ERROR SRAM_Read_SRAM(uint8_t *buffer, uint32_t *buffLen);
MEM_ERROR SRAM_Get_Address(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args);
MEM_ERROR SRAM_Check_Address(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args);
MEM_ERROR SRAM_Check_Address_Range(uint8_t *buffer, uint32_t *buffLen, const uint32_t *args);
MEM_ERROR SRAM_Check_Read_Write_Status(uint8_t *buffer, uint32_t *buffLen);


// helper functions
void clearBuffer(uint8_t index);
void init_counter(void);

void init_arguments(void);
uint8_t get_space(char *rx_buffer);

#define start_timer()    *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
#define stop_timer()   *((volatile uint32_t*)0xE0001000) = 0x40000000  // Disable CYCCNT register
#define get_timer()   *((volatile uint32_t*)0xE0001004)               // Get value from CYCCNT register

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

#endif /* __TUD_SECENG_H */
