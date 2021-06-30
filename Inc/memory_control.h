/**
  ******************************************************************************
  * File Name          : tud_seceng.h
  * Description        : This file contains the common defines of the methods
  ******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TUD_SECENG_H
#define __TUD_SECENG_H

#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "memory_defines.h"


typedef enum {
	FAILED = 0,
	PASSED = !FAILED
} TestStatus; // Typedef for correct read/write tests

typedef enum {
	SHOW_HELP = 0x0,
	FILL_WITH_ZEROS = 0x1,
	FILL_WITH_ONES = 0x2,
	WRITE_ASCENDING = 0x3,
	WRITE_ALTERNATE_ZERO_ONE = 0x4,
	WRITE_ALTERNATE_ONE_ZERO = 0x5,
	WRITE_ADDRESS = 0x6,
	WRITE_ADDRESS_RANGE = 0x7,
	GET_PERFORMANCE_MEASURES = 0x8,
	GET_ADDRESS = 0x9,
	READ = 0xA,
	WRITE = 0xB,
	CHECK_ADDRESS = 0xC,
	CHECK_ADDRESS_RANGE = 0xD,
	GET_VALUES = 0xE
} Command; // List of possible commands

struct
{
	uint8_t wp_enable_Pin;
	uint8_t auto_power_down_enable;
	uint8_t low_power_standby_enable;
	uint8_t block_protection_bits;
	uint8_t write_enable_bit;
	uint8_t write_in_progress_bit;

} typedef MemoryStatusRegister;


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




// uart transmit and receive functions
extern void send(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);
extern void sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);
extern void sendUSB(uint8_t *srcBuffer, uint16_t bufferSize);

extern void receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize);
extern void receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize);


void showHelp(uint8_t *inBuff, uint32_t *buffLen);
extern void showHelpUSB();

void executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBufflen, Command cmdIdx);extern void executeCommandUART(UART_HandleTypeDef *huart, Command idx);
extern void executeCommandUSB();

// functions to access the SRAM
void SRAM_Write_8b(const uint32_t adr, uint8_t value);
uint8_t SRAM_Read_8b(const uint32_t adr);
void SRAM_Write_16b(uint32_t adr, uint16_t value);
uint16_t SRAM_Read_16b(uint32_t adr);


// user functions
void SRAM_Fill_With_Zeros(uint8_t *buffer, uint32_t *buffLen);
void SRAM_Fill_With_Ones(uint8_t *buffer, uint32_t *bufferLen);
void SRAM_Get_Values(uint8_t *buffer, uint32_t *bufferLen);

void SRAM_Get_Performance_Measures(uint8_t *buffer, uint32_t *buffLen);
void SRAM_Write_Ascending(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments);
void SRAM_Write_Alternate_Zero_One(uint8_t *buffer, uint32_t *bufferLen);
void SRAM_Write_Alternate_One_Zero(uint8_t *buffer, uint32_t *bufferLen);
void SRAM_Write_Address(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments);
void SRAM_Write_Address_Range(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments);
void SRAM_Read_SRAM(uint8_t *buffer, uint32_t *buffLen);
void SRAM_Get_Address(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments);
void SRAM_Check_Address(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments);
void SRAM_Check_Address_Range(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments);
void SRAM_Check_Read_Write_Status(uint8_t *buffer, uint32_t *buffLen);


// helper functions
void clearBuffer(uint8_t index);
void init_counter(void);
void tokenize_arguments(char *args);
void init_arguments(void);
uint8_t get_space(char *rx_buffer);

#define start_timer()    *((volatile uint32_t*)0xE0001000) = 0x40000001  // Enable CYCCNT register
#define stop_timer()   *((volatile uint32_t*)0xE0001000) = 0x40000000  // Disable CYCCNT register
#define get_timer()   *((volatile uint32_t*)0xE0001004)               // Get value from CYCCNT register

#ifdef RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
int WIP_Polling();
void Set_WriteEnable();
void Reset_WriteEnable();
void Set_WriteEnableLatch();
void Reset_WriteEnableLatch();
MemoryStatusRegister ReadStatusRegister();

//#endif
#endif //ifdef RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
//delete: CRAP
//void SRAM_Get_Whole_Content(UART_HandleTypeDef *huart);
//uint16_t get_total_flip_probability_16b(uint16_t expected_value, uint16_t real_value);
//uint8_t get_total_flip_probability_8b(uint8_t expected_value, uint8_t real_value);

#endif /* __TUD_SECENG_H */
