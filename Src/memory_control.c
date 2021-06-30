/**
  ******************************************************************************
  * File Name          : tud_seceng.c
  * Description        : Methods for experiments
  ******************************************************************************
*/
#include "memory_control.h"
#include "metrics.h"
#include "spi.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>


//#define strlen(s)   (s)? strlen(s) : 0

/* Create FIFO*/

typedef struct
{
	uint8_t head;
	uint8_t tail;
} FIFO;


FIFO RX_FIFO = {.head = 0, .tail = 0};

char MY_RX_BUFFER[CDC_DATA_HS_MAX_PACKET_SIZE]= {0};

uint8_t* input;


// commands
uint8_t command_mode = 0xFF;
uint8_t write_mode = 0xFF;
uint16_t start_value = 0x0; // start value for ascending writing
uint32_t start_adr = 0x0;	// start address for writeSRAMRange
uint32_t end_adr = 0x0;		// end address for writeSRAMRange
uint32_t arguments[3];
// commands initialization
/*
 * @brief: 	to add new commands, add the command to the array 'command'
 * 			increase the variable 'COMMAND_COUNT' in 'tud_seceng.h'
 * 			add a brief description for the command at the corresponding array index in the array 'command_help'
 *
 * 			Note that the value 0xFF is reserved in the arrays 'command_mode' and 'write_mode'
 *
 * 			If you need more than 3 arguments, increase the array size of 'arguments' to the required value
 */
char * command[] = {
							"help",
							"0",
							"1",
							"writeValueAsc",
							"writeAlternateZeroOne",
							"writeAlternateOneZero",
							"writeSRAM",
							"writeSRAMRange",
							"getPerformanceMeasures",
							"getAddress",
							"readSRAM",
							"checkSRAM",
							"checkAddress",
							"checkAddressRange",
							"v"
						 };

// command help initialization
const char *command_help[] = {
							"'help' Shows this site\r",
							"'0' writes 0's to the whole SRAM\r",
							"'1' writes 1's to the whole SRAM\r",
							"'writeValueAsc val' writes the 8/16-bit value 'val' to the first address and increments the value by 1\r",
							"'writeAlternateZeroOne' writes alternating 01010101... to the whole SRAM\r",
							"'writeAlternateOneZero' writes alternating 10101010... to the whole SRAM\r",
							"'writeSRAM adr val' writes the 8/16-bit value 'val' to address 'adr'\r",
							"'writeSRAMRange start end val' writes the 8/16-bit value 'val' to addresses 'start' to 'end'\r",
							"'getPerformanceMeasures' returns the probabilities P(0->1), P(1->0) and total flip probability (= HammingDistance / SRAM_size)\r",
							"'getAddress adr' returns the value at the address 'adr'\r",
							"'readSRAM' prints the whole content of the SRAM as hexadecimal value to the console (use Putty-logging for file-saving)\r",
							"'checkSRAM' checks, whether the expected value matches the real values of the whole SRAM\r",
							"'checkAddress adr val' checks, whether the value 'val' is equal to the content at address 'adr'\r",
							"'checkAddressRange start end val' checks, whether the value 'val' is equal to the content at the addresses in range 'start' to 'end'\r",
							"'v' returns the number of zeros and ones\n\r"
};



//  OPcode for FUJITSU MB85AS4MT
uint8_t ReRAM_WREN = 0x06;  // Set Write Enable Latch
uint8_t ReRAM_WRDI = 0x04;  // Reset Write Enable Latch
uint8_t ReRAM_RDSR = 0x05;  // Read Status Register
uint8_t ReRAM_WRSR = 0x01;  // Write Status Register
uint8_t ReRAM_READ = 0x03;  // Read Memory Code
uint8_t ReRAM_WRITE = 0x02; // Write Memory Code
uint8_t ReRAM_CERS[] = {0x60, 0xC7};  // Chip Erase
uint8_t ReRAM_PD = 0x09;  // Chip Erase
uint8_t ReRAM_UDPD = 0x79;  // Chip Erase
uint8_t ReRAM_RES = 0xAB;  // Resume from Power Down
uint8_t ReRAM_RDID = 0b10011111;  // Read Device ID
uint8_t ReRAM_SLEEP = 0b10111001; // Sleep Mode

uint8_t ReRam_PowerDown = 0xB9;



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void USBCDCRXCallback(uint8_t *dstBuffer, uint32_t bufferSize);




// weak overload of HAL_Delay, so that interrupt callbacks dont get stuck
void my_HAL_Delay(uint32_t milliseconds)
{
   /* Initially clear flag */
   (void) SysTick->CTRL;
   while (milliseconds != 0) {
      /* COUNTFLAG returns 1 if timer counted to 0 since the last flag read */
      milliseconds -= (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) >> SysTick_CTRL_COUNTFLAG_Pos;
   }
}


// implementation of the functions

//#ifdef SPI
void Set_WriteEnable()
{
	HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_RESET);
}

void Reset_WriteEnable()
{
	HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_RESET);
}

void Set_WriteEnableLatch()
{
	printf("Set Latch\n\n");
	MemoryStatusRegister statusRegister = ReadStatusRegister();
   // PrintStatusRegister(statusRegister);

    Set_WriteEnable();
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &ReRAM_WREN, 1, 1000);
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
	Reset_WriteEnable();

	printf("**END**\n\n");
	statusRegister = ReadStatusRegister();
    //PrintStatusRegister(statusRegister);
}

void Reset_WriteEnableLatch()
{
	Set_WriteEnable();
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &ReRAM_WRDI, 1, 10);
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
	Reset_WriteEnable();
}


MemoryStatusRegister ParseStatusRegister(uint8_t statusRegister) {
	return (MemoryStatusRegister ) { (statusRegister & 128) >> 7,
							   (statusRegister & 64) >> 6,
							   (statusRegister & 32) >> 5,
							   (statusRegister & 12) >> 3,
							   (statusRegister& 2) >> 2,
							   (statusRegister & 1) } ;
		}

void PrintStatusRegister(MemoryStatusRegister reg)
{
	printf("WP_Enalbe: %d\n"
			"Auto_Power_Down_Enable: %d\n"
			"Low_Power_Standby_Enable: %d\n"
			"Block_Protection_Bits: %d\n"
			"Write_Enable_Bits: %d\n"
			"Write_In_Progess_Bits: %d\n",
			reg.wp_enable_Pin,
			reg.auto_power_down_enable,
			reg.low_power_standby_enable,
			reg.block_protection_bits,
			reg.write_enable_bit,
			reg.write_in_progress_bit);
}

//#endif

		/*
 * @brief					writes a 8-Bit(1-byte-word) value to the specified address to SRAM
 * @param uint32_t adr		relative address to the base address (specified as macro 'SRAM_BANK_ADDR') of SRAM
 * 							to be written to
 * @param uint8_t value	    value to be written to the specified address in SRAM
 * @retval					None
 */
void SRAM_Write_8b(const uint32_t adr, uint8_t value)
{
#if MEM_ACCESS_IF==SPI

	Set_WriteEnableLatch();

	// Write Execution
	//#ifdef Adesto
	//uint8_t initialize_write_data[] = {ReRAM_WRITE,((adr >> 16) & 0xFF), ((adr >> 8) & 0xFF), ((adr >>  0) & 0xFF), value};
	uint8_t initialize_write_data[] = {ReRAM_WRITE, ((adr >> 8) & 0xFF), ((adr >>  0) & 0xFF), value};
	Set_WriteEnable();
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_Transmit(&hspi5, initialize_write_data, 4/*260*/, 10);
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
	Reset_WriteEnable();
#endif // MEM_ACCESS_IF == SPI


#if MEM_ACCESS_IF==PARALLEL
	//*(__IO uint8_t *) (SRAM_BANK_ADDR + adr) = value;
#endif //MEM_ACCESS_IF == PARALLEL

}

/*
 * @brief					reads a 8-Bit(1-byte-word) value from the specified address from SRAM
 * @param uint32_t adr		relative address to the base address (specified as macro 'SRAM_BANK_ADDR') of SRAM
 * 							to be read from
 * @retval uint8_t			value at address in SRAM
 */
uint8_t SRAM_Read_8b(const uint32_t adr)
{
#if MEM_ACCESS_IF==SPI
 //   uint8_t read_data[] = {ReRAM_READ,((adr >> 16) & 0xFF),((adr >> 8) & 0xFF),((adr >>  0) & 0xFF)};
	 uint8_t read_data[] = {ReRAM_READ,((adr >> 8) & 0xFF),((adr >>  0) & 0xFF)};
	uint8_t ret_wert[4];
	memset(ret_wert, 0x00, 4);

	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
	//HAL_SPI_TransmitReceive(&hspi5, &read_data,&ret_wert, 5, 10);
	//HAL_SPI_Transmit(&hspi5, read_data, 4, 10);
	HAL_SPI_Transmit(&hspi5, read_data, 3, 100);
	HAL_SPI_Receive(&hspi5, ret_wert, 4, 100);
	HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
#endif // MEM_ACCESS_IF==SPI

//#if MEM_ACCESS_IF==PARALLEL
//    // initialize the return value
//	uint8_t ret_wert;
//
//	//read 8-bit data from memory
//	ret_wert = *(__IO uint8_t *) (SRAM_BANK_ADDR + adr);
//#endif // MEM_ACCESS_IF==PARALLEL

	return ret_wert[0];
}

/*
 * @brief					writes a 16-Bit(2-byte-word) value to the specified address to SRAM
 * @param uint32_t adr		relative address to the base address (specified as macro 'SRAM_BANK_ADDR') of SRAM
 * 							to be written to
 * @param uint16_t value	value to be written to the specified address in SRAM
 * @retval					None
 */
void SRAM_Write_16b(uint32_t adr, uint16_t value){
	// multiply address by 2 due to 2-byte-access (address is given as one byte)
	adr = adr << 1;

	*(__IO uint16_t *) (SRAM_BANK_ADDR + adr) = value;
}

/*
 * @brief					reads a 16-Bit(2-byte-word) value from the specified address from SRAM
 * @param uint32_t adr		relative address to the base address (specified as macro 'SRAM_BANK_ADDR') of SRAM
 * 							to be read from
 * @retval uint16_t			value at address in SRAM
 */
uint16_t SRAM_Read_16b(uint32_t adr){
	// initialize the return value
	uint16_t ret_wert = 0;
	// multiply address by 2 due to 2-byte-access (address is given as one byte)
	adr = adr << 1;

	ret_wert = *(__IO uint16_t *) (SRAM_BANK_ADDR + adr);

	return (ret_wert);
}

/*
 * @brief								fills the whole SRAM with 0's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Fill_With_Zeros(uint8_t *buffer, uint32_t *buffLen){
	write_mode = 0x1;
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value = 0xFFFF;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_16b(adr, 0x0);
			real_value = SRAM_Read_16b(adr);
			// test, if the written value equals the expected value
			// if something went wrong, break here and display an error message
			if(real_value != 0x0){
				state = FAILED;
				break;
			}
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value = 0xFF;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_8b(adr, 0x0);
			real_value = SRAM_Read_8b(adr);
			// test, if the written value equals the expected value
			// if something went wrong, break here and display an error message
			if(real_value != 0x0){
				state = FAILED;
				break;
			}
		}
	}

	// display return message
	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeZeroAll' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeZeroAll' failed\r\n\n");
	}
	sprintf(buffer, "\n\r~~~~~~~~~~~~~~~\n\r"); // FIXME
	*buffLen = strlen(buffer);

//	send(huart, (uint8_t *)STRING_BUFFER, len);
}


/*
 * @brief								fills the whole SRAM with 1's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Fill_With_Ones_8b(UART_HandleTypeDef *huart){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	// variables for measuring write latency
	uint32_t it1, it2;

	int ctr = 0;
	float percentCtr = 1;
	char sendBuffer[4096];
	memset(sendBuffer, 0x00, 4096);

	uint32_t adr = 0; // TODO remove
	//for(uint32_t adr = 0x00; adr < 524288; adr++)
	HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_RESET);

    // Set Write Enable Latch




	while(adr < MEM_SIZE_ADR)
	{
		// Write "010101010101" for initialization
		SRAM_Write_8b(adr,0x55);

		// Wait until the WEL latch turns reset
		WIP_Polling();

		// Write "1010101010"
		SRAM_Write_8b(adr,0xAA);

		// Measure write latency
		int write_latency = 0;
		write_latency = WIP_Polling();

		int ret = SRAM_Read_8b(adr);

		if(ret == 0xAA)
			sprintf(&sendBuffer[strlen(sendBuffer)], "%d; %d\n", adr, write_latency);
		else
			sprintf(&sendBuffer[strlen(sendBuffer)], "%d; %d; %s; %d\n", adr, write_latency, "corrupt", ret);

		if(ctr >= 100)
		{
			ctr = 0;
			sendUART(huart, (uint8_t *)sendBuffer, strlen(sendBuffer));
			memset(sendBuffer, 0x00, 4096);
		}
		ctr++;

		len = strlen(STRING_BUFFER);
		sendUART(huart, (uint8_t *)STRING_BUFFER, len);

		// Check if the writing is executed properly
	/*	uint8_t* real_value = 0xFF;
		real_value = SRAM_Read_8b(adr);

		if(real_value != 0xAA){
		sprintf(STRING_BUFFER, "\n\r  Failed  Adr: %d, Value : %x \n\r", adr, real_value);
		len = strlen(STRING_BUFFER);
		printf(STRING_BUFFER); //send(huart, (uint8_t *)STRING_BUFFER, len);
					//break;
			}*/

			if(adr % 5243 == 0)
			{
				percentCtr+= 0.1;
				printf("Progress %f\n", percentCtr);
			}
			adr++;
		}
		HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_SET);


		if(ctr > 1)
			sendUART(huart, (uint8_t *)sendBuffer, strlen(sendBuffer));


		sprintf(STRING_BUFFER, "\n\rFinished\n\r");
		len = strlen(STRING_BUFFER);
		send(huart, (uint8_t *)STRING_BUFFER, len);
}


/*
 * @brief								fills the whole SRAM with 1's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Fill_With_Ones(uint8_t *buffer, uint32_t *bufferLen){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_16b(adr, 0xFFFF);
			real_value = SRAM_Read_16b(adr);
			// test, if the written value equals the expected value
			// if something went wrong, break here and display an error message
			if(real_value != 0xFFFF){
				state = FAILED;
				break;
			}
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_8b(adr, 0xFF);
			real_value = SRAM_Read_8b(adr);
			//real_value = *(__IO uint8_t *) (SRAM_BANK_ADDR + adr);
			// test, if the written value equals the expected value
			// if something went wrong, break here and display an error message
			if(real_value != 0xFF){
				state = FAILED;
				break;
			}
		}

	}


	// display return message
	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeOneAll' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeOneAll' failed\r\n\n");
	}

	sprintf(buffer, "\n\r~~~~~~~~~~~~~~~\n\r");
	len = strlen(buffer);
	*bufferLen = len;
	//send(huart, (uint8_t *)STRING_BUFFER, len);
}

void SRAM_Get_Values(uint8_t *buffer, uint32_t *bufferLen){
	total_one = 0;
	total_zero = 0;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			uint16_t real_value;
			real_value = SRAM_Read_16b(adr);
			total_one += get_num_one_16b(real_value);
			total_zero += get_num_zero_16b(real_value);
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
				uint8_t real_value;
				real_value = SRAM_Read_8b(adr
						);
				total_one += get_num_one_8b(real_value);
				total_zero += get_num_zero_8b(real_value);
		}
	}

	// displays the total 1's
	sprintf(buffer, "Total number of ones in SRAM:  %lu\r\n", (unsigned long)total_one);
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);

	// displays the total 0's
	sprintf(&buffer[len], "Total number of zeros in SRAM:  %lu\r\n", (unsigned long)total_zero);
	len = strlen(buffer);
	//send(huart, (uint8_t *)buffer, len);

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		// displays the total 0's
		sprintf(&buffer[len], "Percent zeros:  %.16f%%\r\n", (float)total_zero / (MEM_SIZE_ADR * 16) * 100);
		len = strlen(buffer);
		//send(huart, (uint8_t *)buffer, len);

		// displays the total 0's
		sprintf(&buffer[len], "Percent ones:  %.16f%%\r\n", (float)total_one / (MEM_SIZE_ADR * 16) * 100);
		len = strlen(buffer);
		//send(huart, (uint8_t *)buffer, len);
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		// displays the total 0's - (float)total_zero / (SRAM_SIZE * 8) * 100);
		sprintf(&buffer[len], "Percent zeros:  %.8f%%\r\n", (float)total_zero / (MEM_SIZE_ADR * 8) * 100);
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);

		// displays the total 0's - (float)total_one / (SRAM_SIZE * 8) * 100);
		sprintf(&buffer[len], "Percent ones:  %.8f%%\r\n", (float)total_one / (MEM_SIZE_ADR * 8) * 100);
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
	}
	*bufferLen = len;

}

/*
 * @brief								gets the probability of flipped 0's and 1's for the written values
 * 										displays an error message and exits, if no write operation was performed
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Get_Performance_Measures(uint8_t *buffer, uint32_t *buffLen){
	// if no write access was performed => warning
	if(write_mode == 0xFF){
		sprintf(buffer, "Caution. No write operation was performed. Therefore we have no values to compare with...\r\nExit\r\n\n");
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
		return;
	}

	// reset the counter for statistical analysis
	init_counter();
	uint32_t start_local = 0;
	uint32_t end_local = MEM_SIZE_ADR;


	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t expected_value = 0;

		// switch between the different command modes to set the correct values to compare with
		switch(write_mode){
		case 1:
			// writeZeroAll
			expected_value = 0x0;
			break;
		case 2:
			// writeOneAll
			expected_value = 0xFFFF;
			break;
		case 3:
			// writeValueAsc
			expected_value = start_value;
			break;
		case 4:
			// writeAlternateZeroOne
			expected_value = 0x5555;
			break;
		case 5:
			// writeAlternateOneZero
			expected_value = 0xAAAA;
			break;
		case 6:
			// writeSRAM
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		case 7:
			// writeSRAMRange
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		}
		//expected_value = 0x00FF;
		for(uint32_t adr = start_local; adr < end_local; adr++){
			uint16_t real_value;
			real_value = SRAM_Read_16b(adr);
			flipped_one += flipped_one_16b(expected_value, real_value);
			flipped_zero += flipped_zero_16b(expected_value, real_value);
			total_one += get_num_one_16b(expected_value);
			total_zero += get_num_zero_16b(expected_value);
			// if 'writeValueAsc' increment the expected_value
			if(write_mode == 3){
				expected_value++;
			}
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t expected_value = 0;

		// switch between the different command modes to set the correct values to compare with
		switch(write_mode){
		case 1:
			// writeZeroAll
			expected_value = 0x0;
			break;
		case 2:
			// writeOneAll
			expected_value = 0xFF;
			break;
		case 3:
			// writeValueAsc
			expected_value = start_value;
			break;
		case 4:
			// writeAlternateZeroOne
			expected_value = 0x55;
			break;
		case 5:
			// writeAlternateOneZero
			expected_value = 0xAA;
			break;
		case 6:
			// writeSRAM
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		case 7:
			// writeSRAMRange
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		}
		//expected_value = 0x0F;
		for(uint32_t adr = start_local; adr < end_local; adr++){
			uint8_t real_value;
			real_value = SRAM_Read_8b(adr);
			flipped_one += flipped_one_8b(expected_value, real_value);
			flipped_zero += flipped_zero_8b(expected_value, real_value);
			total_one += get_num_one_8b(expected_value);
			total_zero += get_num_zero_8b(expected_value);
			// if 'writeValueAsc' increment the expected_value
			if(write_mode == 3){
				expected_value++;
			}
		}
	}


	// prevent division by zero
	if(total_one == 0){
		sprintf(buffer, "P(1->0) = NaN\r\n");
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
	}else{
		// displays the probability that a 1 changes to a 0
		float p_one_to_zero = ((float)flipped_one / (float)total_one) * 100;
		sprintf(buffer, "P(1->0) = %.16f%%\r\n", p_one_to_zero);
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
	}

	if(total_zero == 0){
		sprintf(&buffer[len], "P(0->1) = NaN\r\n");
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
	}else{
		// displays the probability that a 0 changes to a 1
		float p_zero_to_one = ((float)flipped_zero / (float)total_zero) * 100;
		sprintf(&buffer[len], "P(0->1) = %.16f%%\r\n", p_zero_to_one);
		len = strlen(buffer);
		//send(huart, (uint8_t *)STRING_BUFFER, len);
	}
	float p_total_flip_probability = ((float)(flipped_one + flipped_zero) / (float)(total_one + total_zero)) * 100;

	// displays the total flip probability
	sprintf(&buffer[len], "P(flip) = %.16f%%\r\n", p_total_flip_probability);
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);

	// displays the total flip 1's
	sprintf(&buffer[len], "Total number of flipped 1->0:  %lu\r\n", (unsigned long)flipped_one);
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);

	// displays the total 1's
	sprintf(&buffer[len], "Total number of ones in expected value:  %lu\r\n", (unsigned long)total_one);
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);

	// displays the total flip 0's
	sprintf(&buffer[len], "Total number of flipped 0->1:  %lu\r\n", (unsigned long)flipped_zero);
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);

	// displays the total 0's
	sprintf(&buffer[len], "Total number of zeros in expected value:  %lu\r\n", (unsigned long)total_zero);
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);

	// displays the total flipped bits
	sprintf(&buffer[len], "Total number of flipped bits:  %lu\r\n\n", (unsigned long)(flipped_one + flipped_zero));
	len = strlen(buffer);
	//send(huart, (uint8_t *)STRING_BUFFER, len);
	*buffLen = len;
	/*
	//sprintf(STRING_BUFFER, "total_one = %lu\r\n\n", (unsigned long)total_one);
	sprintf(STRING_BUFFER, "write_mode = %d\r\n\n", write_mode);
	len = strlen(STRING_BUFFER);
	send(huart, (uint8_t *)STRING_BUFFER, len);
	 */

}

/*
 * @brief								fills the first address SRAM with the argument and increment it by one
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			the start value to count up
 */
void SRAM_Write_Ascending(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		start_value = (uint16_t)arguments[0];
		uint16_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_16b(adr, start_value);
			real_value = SRAM_Read_16b(adr);
			if(real_value != start_value){
				state = FAILED;
				break;
			}
			start_value++;
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		start_value = (uint8_t)arguments[0];
		uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_8b(adr, start_value);
			real_value = SRAM_Read_8b(adr);
			if(real_value != start_value){
				state = FAILED;
				break;
			}
			start_value++;
		}
	}

	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeValueAsc' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeValueAsc' failed\r\n\n");
	}
	len = strlen(buffer);
	*buffLen = len;
	//send(huart, (uint8_t *)STRING_BUFFER, len);
}

/*
 * @brief								fills the whole SRAM with alternating 010101010....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Write_Alternate_Zero_One(uint8_t *buffer, uint32_t *bufferLen){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_16b(adr, 0x5555);
			real_value = SRAM_Read_16b(adr);
			if(real_value != 0x5555){
				state = FAILED;
				break;
			}
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_8b(adr, 0x55);
			real_value = SRAM_Read_8b(adr);
			if(real_value != 0x55){
				state = FAILED;
				break;
			}
		}
	}

	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeAlternateZeroOne' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeAlternateZeroOne' failed\r\n\n");
	}
	len = strlen(buffer);

//	send(huart, (uint8_t *)buffer, len);
}

/*
 * @brief								fills the whole SRAM with alternating 101010101....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Write_Alternate_One_Zero(uint8_t *buffer, uint32_t *bufferLen){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_16b(adr, 0xAAAA);
			real_value = SRAM_Read_16b(adr);
			if(real_value != 0xAAAA){
				state = FAILED;
				break;
			}
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value = 0x0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			SRAM_Write_8b(adr, 0xAA);
			real_value = SRAM_Read_8b(adr);
			if(real_value != 0xAA){
				state = FAILED;
				break;
			}
		}
	}

	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeAlternateOneZero' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeAlternateOneZero' failed\r\n\n");
	}
	len = strlen(buffer);
	*bufferLen = len;
	//send(huart, (uint8_t *)buffer, len);
}

/*
 * @brief								writes the value (second entry in parameter arguments) to the
 * 										specified address (first entry in parameter arguments)
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			first element is the address, second element is the value
 */
void SRAM_Write_Address(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = FAILED;

	// set the variables for checking and determining the statistical values
	start_adr = arguments[0];
	end_adr = start_adr + 1;
	uint8_t real_value;


	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		start_value = (uint16_t)arguments[1];
		SRAM_Write_16b(start_adr, start_value);
		uint16_t real_value = SRAM_Read_16b(start_adr);
		if(real_value == start_value){
			state = PASSED;
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		start_value = (uint8_t)arguments[1];

		SRAM_Write_8b(start_adr, start_value);
		uint8_t real_value = SRAM_Read_8b(start_adr);
		if(real_value == start_value){
			state = PASSED;
		}
	}

	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeSRAM' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeSRAM' failed\r\n\n");
	}
	len = strlen(buffer);
	*buffLen = len;
//	send(huart, (uint8_t *)STRING_BUFFER, len);
}

/*
 * @brief								writes the value (third entry in parameter arguments) to the
 * 										specified addresses (from first entry to second entry
 * 										in parameter arguments)
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			first element is the start address, second element is the end address
 * 										third element is the value
 */
void SRAM_Write_Address_Range(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments){
	// reset the counter for statistical analysis
	init_counter();
	// reset the arguments
	init_arguments();

	TestStatus state = PASSED;


	// set the variables for checking and determining the statistical values
	// correct the end_adr for the loop (< not <=)
	start_adr = arguments[0];
	end_adr = arguments[1] + 1;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		start_value = (uint16_t)arguments[2];
		uint16_t real_value;
		if(start_adr < MEM_SIZE_ADR && end_adr <= MEM_SIZE_ADR){
			start_adr = arguments[0];
			end_adr = arguments[1] + 1;
			for(uint32_t adr = start_adr; adr < end_adr; adr++){
				SRAM_Write_16b(adr, start_value);
				real_value = SRAM_Read_16b(adr);
				if(real_value != start_value){
					state = FAILED;
					break;
				}
			}
		}else{
			state = FAILED;
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		start_value = (uint8_t)arguments[2];
		uint8_t real_value;
		if(start_adr < MEM_SIZE_ADR && end_adr <= MEM_SIZE_ADR){
			start_adr = arguments[0];
			end_adr = arguments[1] + 1;
			for(uint32_t adr = start_adr; adr < end_adr; adr++){
				SRAM_Write_8b(adr, start_value);
				real_value = SRAM_Read_8b(adr);
				//real_value = *(__IO uint8_t *) (SRAM_BANK_ADDR + adr);
				if(real_value != start_value){
					state = FAILED;
					break;
				}
			}
		}else{
			state = FAILED;
		}
	}

	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		sprintf(buffer, "'writeSRAMRange' was successful\r\n\n");
	}else{
		sprintf(buffer, "'writeSRAMRange' failed\r\n\n");
	}
	len = strlen(buffer);
	*buffLen = len;
	//send(huart, (uint8_t *)STRING_BUFFER, len);
}

/*
 * @brief								reads the whole SRAM and print it as hexadecimal values
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void SRAM_Read_SRAM(uint8_t *buffer, uint32_t *buffLen){

	srambp = SRAM_BUFFER;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value;
		uint16_t counter = 0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			real_value = SRAM_Read_16b(adr);
			srambp += sprintf(srambp, "%04X\t", real_value);// 0xff 0xffff 0xffffffff
			counter++;
			if(counter == 8192){
				len = strlen(SRAM_BUFFER);
				//send(huart, (uint8_t *)SRAM_BUFFER, len);
				counter = 0;
				srambp = SRAM_BUFFER;
			}
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value;
		uint16_t counter = 0;
		for(uint32_t adr = 0; adr < MEM_SIZE_ADR; adr++){
			real_value = SRAM_Read_8b(adr);
			//real_value = *(__IO uint8_t *) (SRAM_BANK_ADDR + adr);
			srambp += sprintf(srambp, "%02X\t", real_value);
			counter++;
			if(counter == 1024){ //8192//512//1024
				len = strlen(SRAM_BUFFER);
				//send(huart, (uint8_t *)SRAM_BUFFER, len);
				counter = 0;
				srambp = SRAM_BUFFER;
			}
		}
	}

}

/*
 * @brief								prints the address and the value at address (first entry in
 * 										parameter arguments)
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			the address to read from
 */
void SRAM_Get_Address(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments){
	uint32_t adr = arguments[0];

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value = 0;
		real_value = SRAM_Read_16b(adr);
		sprintf(buffer, "Address: %lu\tValue: %#06X\n\n\r", (unsigned long)adr, real_value);
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value = 0;
		real_value = SRAM_Read_8b(adr);
		sprintf(buffer, "Address: %lu\tValue: %#03X\n\n\r", (unsigned long)adr, real_value);
	}

	len = strlen(buffer);
	*buffLen = len;
	//send(huart, (uint8_t *)STRING_BUFFER, len);
}

/*
 * @brief								checks if the expected value (second entry in parameter arguments)
 * 										equals the value stored at address specified in first entry in
 * 										parameter arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			the address to read from, the value to compare with
 */
void SRAM_Check_Address(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments){
	uint32_t adr = arguments[0];
	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t expected_value = (uint16_t)arguments[1];
		uint16_t real_value = SRAM_Read_16b(adr);
		if(real_value != expected_value){
			state = FAILED;
		}
		if(state == PASSED){
			//sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
			sprintf(buffer, "TRUE\n\n\r");
		}else{
			sprintf(buffer, "%#06X != %#06X\n\rFALSE\n\n\r", real_value, expected_value);
		}

	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t expected_value = (uint8_t)arguments[1];
		uint8_t real_value = SRAM_Read_8b(adr);
		if(real_value != expected_value){
			state = FAILED;
		}
		if(state == PASSED){
			//sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
			sprintf(buffer, "TRUE\n\n\r");
		}else{
			sprintf(buffer, "%#03X != %#03X\n\rFALSE\n\n\r", real_value, expected_value);
		}

	}

	len = strlen(buffer);
	*buffLen = len;
	//send(huart, (uint8_t *)STRING_BUFFER, len);
}

/*
 * @brief								checks if the expected value (third entry in parameter arguments)
 * 										equals the value stored at address specified in first entry in
 * 										parameter arguments to address specified in second entry in
 * 										parameter arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			the start address to read from, the end address to read from
 * 										the value to compare with
 */
void SRAM_Check_Address_Range(uint8_t *buffer, uint32_t *buffLen, uint32_t *arguments){
	uint32_t start_local = arguments[0];
	uint32_t end_local = arguments[1];
	TestStatus state = PASSED;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t expected_value_local = (uint16_t)arguments[2];
		uint16_t real_value_local = 0;
		if(start_local <= end_local){
			if(start_local < MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
				for(uint32_t adr = start_local; adr < end_local; adr++){
					real_value_local = SRAM_Read_16b(adr);
					if(real_value_local != expected_value_local){
						state = FAILED;
						sprintf(buffer, "Address: %lu\t%#06X != %#06X\n\r", (unsigned long)adr, real_value_local, expected_value_local);
						len = strlen(buffer);
						//send(huart, (uint8_t *)STRING_BUFFER, len);
					}
				}
			}else{
				state = FAILED;
			}
		}else{
			state = FAILED;
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t expected_value_local = (uint8_t)arguments[2];
		uint8_t real_value_local = 0;
		if(start_local <= end_local){
			if(start_local < MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
				for(uint32_t adr = start_local; adr < end_local; adr++){
					real_value_local = SRAM_Read_8b(adr);
					if(real_value_local != expected_value_local){
						state = FAILED;
						sprintf(buffer, "Address: %lu\t%#03X != %#06X\n\r", (unsigned long)adr, real_value_local, expected_value_local);
						len = strlen(buffer);
					//	send(huart, (uint8_t *)STRING_BUFFER, len);
					}
				}
			}else{
				state = FAILED;
			}
		}else{
			state = FAILED;
		}
	}

	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		//sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
		sprintf(&buffer[len], "TRUE\n\n\r");
	}else{
		sprintf(&buffer[len], "FALSE\n\n\r");
	}
	len = strlen(buffer);
	*buffLen = len;

//	send(huart, (uint8_t *)STRING_BUFFER, len);
}


/*
 * @brief								checks if the expected value (third entry in parameter arguments)
 * 										equals the value stored at address specified in first entry in
 * 										parameter arguments to address specified in second entry in
 * 										parameter arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *arguments			the start address to read from, the end address to read from
 * 										the value to compare with
 */
void SRAM_Check_Read_Write_Status(uint8_t *buffer, uint32_t *buffLen){

	// if no write access was performed => warning
	if(write_mode == 0xFF){
		sprintf(buffer, "Caution. No write access was performed. Therefore we have no values to compare with...\r\nExit\r\n\n");
		len = strlen(buffer);
		*buffLen = len;
		//send(huart, (uint8_t *)STRING_BUFFER, len);
		return;
	}

	TestStatus state = PASSED;
	uint32_t start_local = 0;
	uint32_t end_local = MEM_SIZE_ADR;

	if(MEM_ACCESS_WIDTH_BIT == 16)
	{
		uint16_t real_value = 0;
		uint16_t expected_value = 0;
		// switch between the different command modes to set the correct values to compare with
		switch(write_mode){
		case 1:
			// writeZeroAll
			expected_value = 0x0;
			break;
		case 2:
			// writeOneAll
			expected_value = 0xFFFF;
			break;
		case 3:
			// writeValueAsc
			expected_value = start_value;
			break;
		case 4:
			// writeAlternateZeroOne
			expected_value = 0x5555;
			break;
		case 5:
			// writeAlternateOneZero
			expected_value = 0xAAAA;
			break;
		case 6:
			// writeSRAM
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		case 7:
			// writeSRAMRange
			expected_value = start_value;
			start_local = start_value;
			end_local = end_adr;
			break;
		}

		len = 0;
		if(start_local <= MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
			for(uint32_t adr = start_local; adr < end_local; adr++){
				real_value = SRAM_Read_16b(adr);
				if(real_value != expected_value){
					state = FAILED;
					sprintf(&buffer[len], "Address: %lu\t%#06X != %#06X\n\r", (unsigned long)adr, real_value, expected_value);
					len = strlen(buffer);
					//send(huart, (uint8_t *)STRING_BUFFER, len);
				}
				if(write_mode == 3){
					expected_value++;
				}
			}
		}else{
			state = FAILED;
		}
	}

	if(MEM_ACCESS_WIDTH_BIT == 8)
	{
		uint8_t real_value = 0;
		uint8_t expected_value = 0;
		// switch between the different command modes to set the correct values to compare with
		switch(write_mode){
		case 1:
			// writeZeroAll
			expected_value = 0x0;
			break;
		case 2:
			// writeOneAll
			expected_value = 0xFF;
			break;
		case 3:
			// writeValueAsc
			expected_value = start_value;
			break;
		case 4:
			// writeAlternateZeroOne
			expected_value = 0x55;
			break;
		case 5:
			// writeAlternateOneZero
			expected_value = 0xAA;
			break;
		case 6:
			// writeSRAM
			expected_value = start_value;
			start_local = start_adr;
			end_local = end_adr;
			break;
		case 7:
			// writeSRAMRange
			expected_value = start_value;
			start_local = start_value;
			end_local = end_adr;
			break;
		}

		if(start_local <= MEM_SIZE_ADR && end_local <= MEM_SIZE_ADR){
			for(uint32_t adr = start_local; adr < end_local; adr++){
				real_value = SRAM_Read_8b(adr);
				if(real_value != expected_value){
					state = FAILED;
					sprintf(&buffer[len], "Address: %lu\t%#06X != %#06X\n\r", (unsigned long)adr, real_value, expected_value);
					len = strlen(buffer);
					//send(huart, (uint8_t *)STRING_BUFFER, len);
				}
				if(write_mode == 3){
					expected_value++;
				}
			}
		}else{
			state = FAILED;
		}
		*buffLen = len;
	}


	if(state == PASSED && (MEM_ACCESS_WIDTH_BIT == 8 || MEM_ACCESS_WIDTH_BIT == 16)){
		//sprintf(STRING_BUFFER, "%#06X == %#06X\n\rTRUE\n\n\r", real_value, expected_value);
		sprintf(&buffer[len], "TRUE\n\n\r");
	}else{
		sprintf(&buffer[len], "FALSE\n\n\r");
	}
	len = strlen(buffer);
	*buffLen = len;
	//send(huart, (uint8_t *)STRING_BUFFER, len);
}

/*
 * @brief								rewritten function to receive with a delay of 10ms
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 * @param uint8_t *dstBuffer			the destination buffer
 * @param uint32_t bufferSize			the buffer size
 */
void receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize){
	HAL_UART_Receive_IT(huart, dstBuffer, bufferSize);
	HAL_Delay(10);
}


/*
 * @brief								rewritten function to receive with a delay of 10ms
 * @param uint8_t *dstBuffer			the destination buffer
 * @param uint32_t bufferSize			the buffer size
 */
/*
void receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize)
{
	//CDC_Receive_HS(dstBuffer, bufferSize);
	my_HAL_Delay(10);
}
*/

void USBCDCRXCallback(uint8_t* Buf, uint32_t Len)
//uint8_t VCP_read(uint8_t* Buf, uint32_t Len)
{
	//uint32_t count=0;
	//uint8_t myBuf = *Buf;
	//char myChar = (char) myBuf;

	memcpy(MY_RX_BUFFER,Buf,Len);
	uint32_t MY_RX_BUFFER_LEN = Len;

	/* Check inputs */
	if ((Buf == NULL) || (Len == 0))
	{
		return;// 0;
	}
	else
	{
		for(uint8_t i=0; i<MY_RX_BUFFER_LEN; i++)
		{
			Rx_Data[0] = MY_RX_BUFFER[i];

			//Rx_Data[0] = (char)(*Buf++);

			if (Rx_Index == 0)
			{
				clearBuffer(0);
			}

			// if received data different from ascii 13 (Enter)
			if (Rx_Data[0] != 13)
			{
				// only allow 0-9, A-Z, a-z, SP (32), DEL (127)
				if (Rx_Data[0] == 32 || Rx_Data[0] == 127
						|| (Rx_Data[0] > 47 && Rx_Data[0] < 58)
						|| (Rx_Data[0] > 64 && Rx_Data[0] < 91)
						|| (Rx_Data[0] > 96 && Rx_Data[0] < 123))
				{
					if (Rx_Data[0] == 127)
					{
						if (Rx_Index > 0)
						{
							Rx_Index--;
						}
						// correct extended ascii chars which uses two bytes when press 'Delete'
						if (Rx_Buffer[Rx_Index] > 127 && Rx_Index > 0)
						{
							Rx_Index--;
						}
					}
					else
					{
						Rx_Buffer[Rx_Index++] = Rx_Data[0];
					}
					// print input char by char

					sprintf(STRING_BUFFER, "%c", Rx_Data[0]);
					len = strlen(STRING_BUFFER);
					sendUSB((uint8_t *)STRING_BUFFER, len);
				}
			}
			else
			{
//				sprintf(STRING_BUFFER, "Detected carriage Return\r\n");
//				len = strlen(STRING_BUFFER);
//				sendUSB((uint8_t *)STRING_BUFFER, len);

				// if received data = 13
				Rx_Index = 0;
				Transfer_cplt = 1; // transfer complete, data is ready to read
			}

		}

	}
}


		//HAL_UART_Receive_IT(huart, (uint8_t*) Rx_Data, 1);


/*
		while (Len--)
		{
			if (RX_FIFO.head==RX_FIFO.tail);// return count;
			count++;
			*Buf++=RX_FIFO.data[RX_FIFO.tail];
			RX_FIFO.tail=FIFO_INCR(RX_FIFO.tail);
		}
*/



//	// clear Rx_Buffer before receiving new data
//	if (huart->Instance == UART4)
//	{
//		if (Rx_Index == 0)
//		{
//			clearBuffer(0);
//		}
//
//		// if received data different from ascii 13 (Enter)
//		if (Rx_Data[0] != 13)
//		{
//			// only allow 0-9, A-Z, a-z, SP (32), DEL (127)
//			if (Rx_Data[0] == 32 || Rx_Data[0] == 127
//					|| (Rx_Data[0] > 47 && Rx_Data[0] < 58)
//					|| (Rx_Data[0] > 64 && Rx_Data[0] < 91)
//					|| (Rx_Data[0] > 96 && Rx_Data[0] < 123))
//			{
//				if (Rx_Data[0] == 127)
//				{
//					if (Rx_Index > 0)
//					{
//						Rx_Index--;
//					}
//					// correct extended ascii chars which uses two bytes when press 'Delete'
//					if (Rx_Buffer[Rx_Index] > 127 && Rx_Index > 0)
//					{
//						Rx_Index--;
//					}
//					//clearBuffer(Rx_Index);
//				}
//				else
//				{
//					Rx_Buffer[Rx_Index++] = Rx_Data[0];
//				}
//				// print input char by char
//				HAL_UART_Transmit_IT(huart, (uint8_t*) Rx_Data, 1);
//			}
//		}
//		else
//		{
//			// if received data = 13
//			Rx_Index = 0;
//			Transfer_cplt = 1; // transfer complete, data is ready to read
//		}
//
//		HAL_UART_Receive_IT(huart, (uint8_t*) Rx_Data, 1);
//	}











/*
* @brief								rewritten function to receive with a delay of 10ms
* @param uint8_t *dstBuffer			the destination buffer
* @param uint32_t bufferSize			the buffer size
*/
//void USBCDCReceiveCallback(uint8_t *dstBuffer, uint32_t bufferSize)
//{
//	// clear Rx_Buffer before receiving new data
//
//	//if(huart->Instance == UART4){
//
//		memcpy(dstBuffer, Rx_Buffer, bufferSize);
//
//		/*if(Rx_Index == 0){
//			clearBuffer(0);
//		}*/
//
//		// if received data different from ascii 13 (Enter)
//		if(Rx_Data[0] != 13){
//			// only allow 0-9, A-Z, a-z, SP (32), DEL (127)
//			if(Rx_Data[0] == 32 || Rx_Data[0] == 127 || (Rx_Data[0] > 47 && Rx_Data[0] < 58) || (Rx_Data[0] > 64 && Rx_Data[0] < 91) ||
//					(Rx_Data[0] > 96 && Rx_Data[0] < 123)){
//				if(Rx_Data[0] == 127){
//					if(Rx_Index > 0){
//						Rx_Index--;
//					}
//					// correct extended ascii chars which uses two bytes when press 'Delete'
//					if(Rx_Buffer[Rx_Index] > 127 && Rx_Index > 0){
//						Rx_Index--;
//					}
//					//clearBuffer(Rx_Index);
//				}else{
//					Rx_Buffer[Rx_Index++] = Rx_Data[0];
//				}
//				// print input char by char
//				//CDC_Transmit_HS((uint8_t *)Rx_Data, 1);
//			}
//		}else{
//			// if received data = 13
//			Rx_Index = 0;
//			Transfer_cplt = 1; // transfer complete, data is ready to read
//		}
//
//	//}
//}


void send(UART_HandleTypeDef *huart, uint8_t *sendBuffer, uint32_t bufferSize)
{
	printf((char*)sendBuffer);
	if(huart == NULL)
	{
		sendUSB(sendBuffer, bufferSize);
	}
	else
	{
		sendUART(huart, sendBuffer, bufferSize);
	}

}


/*
 * @brief								rewritten function to transmit with a delay of 10ms
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 * @param uint8_t *dstBuffer			the source buffer
 * @param uint32_t bufferSize			the buffer size
 */
void sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize){
	HAL_UART_Transmit_IT(huart, srcBuffer, bufferSize);
	if(command_mode == 0xA){
		HAL_Delay(3500);
	}else{
		HAL_Delay(20);
	}
}




/*
 * @brief								rewritten function to transmit with a delay of 10ms
 * @param uint8_t *dstBuffer			the source buffer
 * @param uint32_t bufferSize			the buffer size
 */
void sendUSB(uint8_t *srcBuffer, uint16_t bufferSize){
	//CDC_Receive_HS(input,1);
	//USBD_CDC_ReceivePacket(&hUsbDeviceHS);
	//flushRead();

	uint8_t result = USBD_OK;
	do
	{
		result = CDC_Transmit_HS(srcBuffer, bufferSize);
		my_HAL_Delay(10);
	}
	while(result != USBD_OK);
}


/*
 * @brief								prints the help for the commands
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void showHelp(uint8_t *buffer, uint32_t *bufferLen){
	uint32_t len;
	sprintf(buffer, "\rThis program provides the following commands:\n\n\r");
	len = strlen(buffer);
//	send(huart, (uint8_t *)STRING_BUFFER, len);
	for(uint8_t i = 0; i < COMMAND_COUNT; i++){
		sprintf(&(buffer[len]), "%s\r\n", command_help[i]);
		len = strlen(buffer);
		*bufferLen = len;
		// TODO check bufferLen
		//send(huart, (uint8_t *)STRING_BUFFER, len);
	}
}

/*
 * @brief								parse the command, set the required variables, parse the arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
//void executeCommandUSB(){
//	command_mode = 0xFF; // invalid command
//	// parse command
//	for(uint8_t i = 0; i < COMMAND_COUNT; i++)
//	{
//		// check if the command equals a command specified in the array 'command'
//		// if so set command_mode different from 0xFF
//		uint8_t command_end_index = get_space(Rx_Buffer);
//		if((uint8_t)strlen(command[i]) == command_end_index && strncmp(command[i], Rx_Buffer, command_end_index) == 0)
//		{
//			command_mode = i;
//			char tmp[30];
//			uint16_t len_rx_buffer = strlen(Rx_Buffer);
//			uint16_t len_command = strlen(command[i]);
//			// if there are arguments after the command
//			if(len_rx_buffer - len_command > 0)
//			{
//				// extract the arguments from the string
//				strncpy(tmp, Rx_Buffer + len_command, len_rx_buffer - len_command);
//				// tokenize the arguments and fill the array 'arguments'
//				tokenize_arguments(tmp);
//			}
//			break;
//		}
//	}
//
//	//sprintf(STRING_BUFFER, "Length: %d, String: %d %d %d %d\r\n", (uint16_t)strlen(Rx_Buffer), Rx_Buffer[0], Rx_Buffer[1], Rx_Buffer[2], Rx_Buffer[3]);
//	// prints the command
//	sprintf(STRING_BUFFER, ">\r\n");
//	len = strlen(STRING_BUFFER);
//	//flushRead();
//	//HAL_Delay(3500);
//	sendUSB((uint8_t *)STRING_BUFFER, len);
//	//CDC_Transmit_HS((uint8_t *)STRING_BUFFER, len);
//
//	// reset the counter before every write execution
//	//
//	switch(command_mode){
//	case 0x0:
//		// no write operation will be performed in this method
//		//write_mode = 0xFF;
//		return showHelp(NULL);
//	case 0x1:
//		// write operation in mode 1 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x1;
//		return SRAM_Fill_With_Zeros(NULL);
//	case 0x2:
//		// write operation in mode 2 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x2;
//		return SRAM_Fill_With_Ones(NULL);
//	case 0x3:
//		// write operation in mode 3 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x3;
//		return SRAM_Write_Ascending(NULL, arguments);
//	case 0x4:
//		// write operation in mode 4 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x4;
//		return SRAM_Write_Alternate_Zero_One(NULL);
//	case 0x5:
//		// write operation in mode 5 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x5;
//		return SRAM_Write_Alternate_One_Zero(NULL);
//	case 0x6:
//		// write operation in mode 6 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x6;
//		return SRAM_Write_Address(NULL, arguments);
//	case 0x7:
//		// write operation in mode 7 will be performed in this method
//		// therefore reset the counters/arguments
//		// they will be set in the function
//		write_mode = 0x7;
//		return SRAM_Write_Address_Range(NULL, arguments);
//	case 0x8:
//		// no write operation will be performed in this method
//		// reset the counter for statistical analysis
//		//write_mode = 0xFF;
//		return SRAM_Get_Performance_Measures(NULL);
//	case 0x9:
//		// no write operation will be performed in this method
//		// reset the counter for statistical analysis
//		//write_mode = 0xFF;
//		return SRAM_Get_Address(NULL, arguments);
//	case 0xA:
//		// no write operation will be performed in this method
//		//write_mode = 0xFF;
//		return SRAM_Read_SRAM(NULL);
//		break;
//	case 0xB:
//		// no write operation will be performed in this method
//		//write_mode = 0xFF;
//		return SRAM_Check_Read_Write_Status(NULL);
//	case 0xC:
//		// no write operation will be performed in this method
//		//write_mode = 0xFF;
//		return SRAM_Check_Address(NULL, arguments);
//	case 0xD:
//		// no write operation will be performed in this method
//		//write_mode = 0xFF;
//		return SRAM_Check_Address_Range(NULL, arguments);
//	case 0xE:
//		return SRAM_Get_Values(NULL);
//	default:
//		sprintf(STRING_BUFFER, "Command not found. Type 'help' to show all valid commands.\n\n\r");
//		len = strlen(STRING_BUFFER);
//		sendUSB((uint8_t *)STRING_BUFFER, len);
//	}
//}

void executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBuffLen, Command cmdIdx)
{
	UART_HandleTypeDef *huart = NULL;
	switch(cmdIdx){
		case SHOW_HELP:
			// no write operation will be performed in this method
			//write_mode = 0xFF;
			return showHelp(outBuff, outBuffLen);
		case FILL_WITH_ZEROS:
			// write operation in mode 1 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x1;
			return SRAM_Fill_With_Zeros(outBuff, outBuffLen);
		case FILL_WITH_ONES:
			// write operation in mode 2 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x2;
			return SRAM_Fill_With_Ones(outBuff, outBuffLen);
		case WRITE_ASCENDING:
			// write operation in mode 3 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x3;
			return SRAM_Write_Ascending(outBuff, outBuffLen, arguments);
		case WRITE_ALTERNATE_ZERO_ONE:
			// write operation in mode 4 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x4;
			return SRAM_Write_Alternate_Zero_One(outBuff, outBuffLen);
		case WRITE_ALTERNATE_ONE_ZERO:
			// write operation in mode 5 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x5;
			return SRAM_Write_Alternate_One_Zero(outBuff, outBuffLen);
		case WRITE_ADDRESS:
			// write operation in mode 6 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x6;
			return SRAM_Write_Address(outBuff, outBuffLen, arguments);
		case WRITE_ADDRESS_RANGE:
			// write operation in mode 7 will be performed in this method
			// therefore reset the counters/arguments
			// they will be set in the function
			write_mode = 0x7;
			return SRAM_Write_Address_Range(outBuff, outBuffLen, arguments);
		case GET_PERFORMANCE_MEASURES:
			// no write operation will be performed in this method
			// reset the counter for statistical analysis
			//write_mode = 0xFF;
			return SRAM_Get_Performance_Measures(outBuff, outBuffLen);
		case GET_ADDRESS:
			// no write operation will be performed in this method
			// reset the counter for statistical analysis
			//write_mode = 0xFF;
			return SRAM_Get_Address(outBuff, outBuffLen, arguments);
		case READ:
			// no write operation will be performed in this method
			//write_mode = 0xFF;
			return SRAM_Read_SRAM(outBuff, outBuffLen);
			break;
		case WRITE:
			// no write operation will be performed in this method
			//write_mode = 0xFF;
			return SRAM_Check_Read_Write_Status(outBuff, outBuffLen);
		case CHECK_ADDRESS:
			// no write operation will be performed in this method
			//write_mode = 0xFF;
			return SRAM_Check_Address(outBuff, outBuffLen, arguments);
		case CHECK_ADDRESS_RANGE:
			// no write operation will be performed in this method
			//write_mode = 0xFF;
			return SRAM_Check_Address_Range(outBuff, outBuffLen, arguments);
		case GET_VALUES:
			return SRAM_Get_Values(outBuff, outBuffLen);
		default:
			sprintf(STRING_BUFFER, "Command not found. Type 'help' to show all valid commands.\n\n\r");
			len = strlen(STRING_BUFFER);

			send(huart, (uint8_t *)STRING_BUFFER, len);
		}
}


/*
 * @brief								parse the command, set the required variables, parse the arguments
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void executeCommandUART(UART_HandleTypeDef *huart, Command cmdIdx){
	command_mode = 0xFF; // invalid command
		// parse command
		for(uint8_t i = 0; i < COMMAND_COUNT; i++){
			// check if the command equals a command specified in the array 'command'
			// if so set command_mode different from 0xFF
			uint8_t command_end_index = get_space(Rx_Buffer);
			if((uint8_t)strlen(command[i]) == command_end_index && strncmp(command[i], Rx_Buffer, command_end_index) == 0){
				command_mode = i;
				char tmp[30];
				uint16_t len_rx_buffer = strlen(Rx_Buffer);
				uint16_t len_command = strlen(command[i]);
				// if there are arguments after the command
				if(len_rx_buffer - len_command > 0){
					// extract the arguments from the string
					strncpy(tmp, Rx_Buffer + len_command, len_rx_buffer - len_command);
					// tokenize the arguments and fill the array 'arguments'
					tokenize_arguments(tmp);
				}
				break;
			}
		}

		//sprintf(STRING_BUFFER, "Length: %d, String: %d %d %d %d\r\n", (uint16_t)strlen(Rx_Buffer), Rx_Buffer[0], Rx_Buffer[1], Rx_Buffer[2], Rx_Buffer[3]);
		// prints the command
		sprintf(STRING_BUFFER, ">\r\n");
		len = strlen(STRING_BUFFER);
		send(huart, (uint8_t *)STRING_BUFFER, len);

		// reset the counter before every write execution
		uint32_t len = BUFFER_SIZE;
		executeCommand(arguments, 0 /*TODO*/, STRING_BUFFER, &len, command_mode);
		if(len > 0)
			send(huart, (uint8_t *)STRING_BUFFER, len);
}


/*
 * @brief								callback function called if something is received via UART
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	// clear Rx_Buffer before receiving new data
	if(huart->Instance == UART4){
		if(Rx_Index == 0){
			clearBuffer(0);
		}
		// if received data different from ascii 13 (Enter)
		if(Rx_Data[0] != 13){
			// only allow 0-9, A-Z, a-z, SP (32), DEL (127)
			if(Rx_Data[0] == 32 || Rx_Data[0] == 127 || (Rx_Data[0] > 47 && Rx_Data[0] < 58) || (Rx_Data[0] > 64 && Rx_Data[0] < 91) ||
					(Rx_Data[0] > 96 && Rx_Data[0] < 123)){
				if(Rx_Data[0] == 127){
					if(Rx_Index > 0){
						Rx_Index--;
					}
					// correct extended ascii chars which uses two bytes when press 'Delete'
					if(Rx_Buffer[Rx_Index] > 127 && Rx_Index > 0){
						Rx_Index--;
					}
					//clearBuffer(Rx_Index);
				}else{
					Rx_Buffer[Rx_Index++] = Rx_Data[0];
				}
				// print input char by char
				HAL_UART_Transmit_IT(huart, (uint8_t *)Rx_Data, 1);
			}
		}else{
			// if received data = 13
			Rx_Index = 0;
			Transfer_cplt = 1; // transfer complete, data is ready to read
		}
		HAL_UART_Receive_IT(huart, (uint8_t *)Rx_Data, 1);
	}
}

/*
 * @brief					clears the rx_buffer starting from address specified in param index
 * @param uint8_t index		index to start from clearing the buffer
 */
void clearBuffer(uint8_t index){
	for(uint8_t i = index; i < BUFFER_SIZE; i++){
		Rx_Buffer[i] = 0;
	}
}

/*
 * @brief					tokenizes the arguments from the commands and fills the array 'arguments'
 * @param					the string with the arguments
 */
void tokenize_arguments(char* args){
	char* token = strtok(args, " ");
	uint8_t i = 0;
	while(token){
		arguments[i] = (uint32_t)strtol(token, NULL, 16);
		token = strtok(NULL, " ");
		i++;
	}
}


/*
 * @brief							resets the counters for statistical analysis
 */
void init_counter(){
	total_one = 0;
	total_zero = 0;
	flipped_one = 0;
	flipped_zero = 0;
}

/*
 * @brief							resets the custom start/end addresses and the custom start value
 * 									these variables are set in a write method
 */
void init_arguments(){
	start_adr = 0;
	end_adr = 0;
	start_value = 0;
}

uint8_t get_space(char *rx_buffer)
{
	for(uint8_t i = 0; i < strlen(rx_buffer); i++)
	{
		if(rx_buffer[i] == 32)
		{
			return i;
		}
	}
	return (uint8_t)strlen(rx_buffer);
}

//DELETEABLE CRAP
/*
 * @brief								determines the number of different bits in a 16 bit string compared
 * 										to the expected value
 * @param uint16_t expected_value		the expected value
 * @param uint16_t real_value			the (possibly changed) value from the SRAM
 */
/*
uint16_t get_total_flip_probability_16b(uint16_t expected_value, uint16_t real_value){
	uint16_t bitstring = expected_value ^ real_value;

	return (hamming_weight_16b(bitstring));
}
*/

/*
 * @brief								determines the number of different bits in a 8 bit string compared
 * 										to the expected value
 * @param uint8_t expected_value		the expected value
 * @param uint8_t real_value			the (possibly changed) value from the SRAM
 */
/*
uint8_t get_total_flip_probability_8b(uint8_t expected_value, uint8_t real_value){
	uint8_t bitstring = expected_value ^ real_value;

	return (hamming_weight_8b(bitstring));
}
*/

MemoryStatusRegister ReadStatusRegister()
{
	uint8_t reg;
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi5, &ReRAM_RDSR, 1, 10);
    HAL_SPI_Receive(&hspi5, &reg, 1, 10);
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
    return ParseStatusRegister(reg);
}


void WriteStatusRegister()
{

}


#ifdef RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
int WIP_Polling(){

    uint8_t reply;
    int it1, it2;

    //start timer
    start_timer();
    it1 = get_timer();

    int count = 0;
    reply = 0xFF;

    while(1 > 0) {
        count += 1;
        MemoryStatusRegister statusRegister = ReadStatusRegister();
        if (statusRegister.write_in_progress_bit == 0) {
        	stop_timer();
            it2 = get_timer() - it1;
            break;
        }
    }

    //return it2;
    return it2;
}
#endif // #ifdef RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
