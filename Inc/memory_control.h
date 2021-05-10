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


typedef enum {
	FAILED = 0,
	PASSED = !FAILED
} TestStatus; // Typedef for correct read/write tests


// Memory Definitions: Select only one! Note: 0°C = 273°K
//#define SRAM_ALLIANCE_AS7C34098A
//#define SRAM_ALLIANCE_AS7C38098A
//#define SRAM_CYPRESS_CY62157EV30
//#define SRAM_CYPRESS_CY7C1041CV33
//#define SRAM_CYPRESS_CY7C1051DV33
//#define SRAM_ISSI_IS61WV51216BLL
//#define MRAM_EVERSPIN_MR4A08BYS35
//#define MRAM_EVERSPIN_MR4A08BUYS45
#define FRAM_CYPRESS_FM22L16_55_TG
//#define FRAM_ROHM_MR48V256CTAZAARL
//#define FRAM_FUJITSU_MB85R1001ANC_GE1
//#define RERAM_ADESTO_RM25C512C_LTAI_T
//#define RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1

#ifdef SRAM_ALLIANCE_AS7C34098A

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)262144)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)12)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_ALLIANCE_AS7C38098A

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)10)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2700)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_CYPRESS_CY62157EV30

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)45)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2200)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_CYPRESS_CY7C1041CV33

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)262144)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)8)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_CYPRESS_CY7C1051DV33

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)10)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_ISSI_IS61WV51216BLL

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)10)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2400)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef MRAM_EVERSPIN_MR4A08BYS35

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)2097152)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)35)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)273)
#define MEM_OP_TEMP_MIN_K			((uint16_t)343)

#endif

#ifdef MRAM_EVERSPIN_MR4A08BUYS45

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)2097152)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)45)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)398)

#endif

#ifdef FRAM_CYPRESS_FM22L16_55_TG

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)262144)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)55)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2700)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef FRAM_ROHM_MR48V256CTAZAARL

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)32768)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)70)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2700)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef FRAM_FUJITSU_MB85R1001ANC_GE1

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)131072)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)150)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef RERAM_ADESTO_RM25C512C_LTAI_T

#define MEM_ACCESS_IF 				SPI
#define MEM_SIZE_ADR 				((uint32_t)512)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)1024)
#define MEM_ACCESS_TIME_NS			((uint32_t)150)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)1650)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1

#define MEM_ACCESS_IF 				SPI
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)150)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)300)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif



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

//char hallo[12];



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


extern void showHelp(UART_HandleTypeDef *huart);
extern void showHelpUSB();

extern void executeCommand(UART_HandleTypeDef *huart, int idx);
extern void executeCommandUSB();

// functions to access the SRAM
void SRAM_Write_8b(uint32_t adr, uint8_t value);
uint8_t SRAM_Read_8b(uint32_t adr);
void SRAM_Write_16b(uint32_t adr, uint16_t value);
uint16_t SRAM_Read_16b(uint32_t adr);


// user functions
void SRAM_Fill_With_Zeros(UART_HandleTypeDef *huart);
void SRAM_Fill_With_Ones(UART_HandleTypeDef *huart);
void SRAM_Get_Values(UART_HandleTypeDef *huart);

void SRAM_Get_Performance_Measures(UART_HandleTypeDef *huart);
void SRAM_Write_Ascending(UART_HandleTypeDef *huart, uint32_t *arguments);
void SRAM_Write_Alternate_Zero_One(UART_HandleTypeDef *huart);
void SRAM_Write_Alternate_One_Zero(UART_HandleTypeDef *huart);
void SRAM_Write_Address(UART_HandleTypeDef *huart, uint32_t *arguments);
void SRAM_Write_Address_Range(UART_HandleTypeDef *huart, uint32_t *arguments);
void SRAM_Read_SRAM(UART_HandleTypeDef *huart);
void SRAM_Get_Address(UART_HandleTypeDef *huart, uint32_t *arguments);
void SRAM_Check_Address(UART_HandleTypeDef *huart, uint32_t *arguments);
void SRAM_Check_Address_Range(UART_HandleTypeDef *huart, uint32_t *arguments);
void SRAM_Check_Read_Write_Status(UART_HandleTypeDef *huart);


// helper functions
void clearBuffer(uint8_t index);
uint16_t hamming_weight_16b(uint16_t bitstring);
uint8_t hamming_weight_8b(uint8_t bitstring);
void init_counter(void);
void tokenize_arguments(char *args);
uint16_t get_num_one_16b(uint16_t bitstring);
uint16_t get_num_zero_16b(uint16_t bitstring);
uint8_t get_num_one_8b(uint8_t bitstring);
uint8_t get_num_zero_8b(uint8_t bitstring);
uint16_t flipped_one_16b(uint16_t expected_value, uint16_t real_value);
uint16_t flipped_zero_16b(uint16_t expected_value, uint16_t real_value);
uint8_t flipped_one_8b(uint8_t expected_value, uint8_t real_value);
uint8_t flipped_zero_8b(uint8_t expected_value, uint8_t real_value);

void init_arguments(void);
uint8_t get_space(char *rx_buffer);

//delete: CRAP
//void SRAM_Get_Whole_Content(UART_HandleTypeDef *huart);
//uint16_t get_total_flip_probability_16b(uint16_t expected_value, uint16_t real_value);
//uint8_t get_total_flip_probability_8b(uint8_t expected_value, uint8_t real_value);

#endif /* __TUD_SECENG_H */
