//
// Created by florianfrank on 01.07.21.
//
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <SystemFiles/usbd_cdc_if.h>
#include "SystemFiles/spi.h"
#include "metrics.h"
#include "cpp/MemoryController.h"
#include "cpp/CommandLineParser.h"


/*static*/ const char* CommandLineParser::command[] = {
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

/*static*/ const char* CommandLineParser::command_help[] = {
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


CommandLineParser::CommandLineParser(MemoryController &memoryController): m_MemoryController(memoryController)
{}

// uart transmit and receive functions
extern void send(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);

extern void sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);

extern void sendUSB(uint8_t *srcBuffer, uint16_t bufferSize);

void tokenize_arguments(char *args);

/*
 * @brief								prints the help for the commands
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
void CommandLineParser::showHelp(uint8_t *buffer, uint32_t *bufferLen){
    uint32_t buffLen;
    sprintf((char*)buffer, "\rThis program provides the following commands:\n\n\r");
    buffLen = strlen((char*)buffer);
//	send(huart, (uint8_t *)STRING_BUFFER, buffLen);
    for(uint8_t i = 0; i < COMMAND_COUNT; i++){
        sprintf((char*)&(buffer[buffLen]), "%s\r\n", command_help[i]);
        buffLen = strlen((char*)buffer);
        *bufferLen = buffLen;
        // TODO check bufferLen
        //send(huart, (uint8_t *)STRING_BUFFER, buffLen);
    }
}

MEM_ERROR CommandLineParser::executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBuffLen, Command cmdIdx)
{
    UART_HandleTypeDef *huart = NULL;
    switch(cmdIdx){
        case SHOW_HELP:
            // no write operation will be performed in this method
            //write_mode = 0xFF;
            showHelp(outBuff, outBuffLen);
        case FILL_WITH_ZEROS:
            // write operation in mode 1 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x1;
            return m_MemoryController.MemoryFillWithZeros(outBuff, outBuffLen);
        case FILL_WITH_ONES:
            // write operation in mode 2 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x2;
            return m_MemoryController.MemoryFillWithOnes(outBuff, outBuffLen);
        case WRITE_ASCENDING:
            // write operation in mode 3 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x3;
            return m_MemoryController.MemoryFillMemoryWithAscendingValues(outBuff, outBuffLen, arguments);
        case WRITE_ALTERNATE_ZERO_ONE:
            // write operation in mode 4 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x4;
            return m_MemoryController.MemoryWriteAlternatingZeroAndOne(outBuff, outBuffLen);
        case WRITE_ALTERNATE_ONE_ZERO:
            // write operation in mode 5 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x5;
            return m_MemoryController.MemoryWriteAlternatingOneAndZero(outBuff, outBuffLen);
        case WRITE_ADDRESS:
            // write operation in mode 6 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x6;
            return m_MemoryController.MemoryWriteSingleValue(outBuff, outBuffLen, arguments);
        case WRITE_ADDRESS_RANGE:
            // write operation in mode 7 will be performed in this method
            // therefore reset the counters/arguments
            // they will be set in the function
            write_mode = 0x7;
            return m_MemoryController.MemoryWriteAddressRange(outBuff, outBuffLen, arguments);
        case GET_PERFORMANCE_MEASURES:
            // no write operation will be performed in this method
            // reset the counter for statistical analysis
            //write_mode = 0xFF;
            return m_MemoryController.MemoryGetProbabilityOfFlippedOnesAndZeros(outBuff, outBuffLen);
        case GET_ADDRESS:
            // no write operation will be performed in this method
            // reset the counter for statistical analysis
            //write_mode = 0xFF;
            return m_MemoryController.SRAM_Get_Address(outBuff, outBuffLen, arguments);
        case READ:
            // no write operation will be performed in this method
            //write_mode = 0xFF;
            return m_MemoryController.MemoryReadWholeMemory(outBuff, outBuffLen);
        case WRITE:
            // no write operation will be performed in this method
            //write_mode = 0xFF;
            return m_MemoryController.SRAM_Check_Read_Write_Status(outBuff, outBuffLen);
        case CHECK_ADDRESS:
            // no write operation will be performed in this method
            //write_mode = 0xFF;
            return m_MemoryController.SRAM_Check_Address(outBuff, outBuffLen, arguments);
        case CHECK_ADDRESS_RANGE:
            // no write operation will be performed in this method
            //write_mode = 0xFF;
            return m_MemoryController.SRAM_Check_Address_Range(outBuff, outBuffLen, arguments);
        case GET_VALUES:
            return m_MemoryController.MemoryReadArea(outBuff, outBuffLen);
        default:
            sprintf(STRING_BUFFER, "Command not found. Type 'help' to show all valid commands.\n\n\r");
            len = strlen(STRING_BUFFER);
            send(huart, (uint8_t *)STRING_BUFFER, len);
            return MemoryErrorHandling::MEM_INVALID_COMMAND;
    }
}

__unused void CommandLineParser::executeCommandUART(UART_HandleTypeDef *huart, Command cmdIdx){
    command_mode = NOPE; // invalid command
    // parse command
    for(uint8_t i = 0; i < COMMAND_COUNT; i++){
        // check if the command equals a command specified in the array 'command'
        // if so set command_mode different from 0xFF
        uint8_t command_end_index = m_MemoryController.get_space(Rx_Buffer);
        if((uint8_t)strlen(command[i]) == command_end_index && strncmp(command[i], Rx_Buffer, command_end_index) == 0){
            command_mode = static_cast<Command>(i);
            uint16_t len_rx_buffer = strlen(Rx_Buffer);
            uint16_t len_command = strlen(command[i]);
            // if there are arguments after the command
            if(len_rx_buffer - len_command > 0){
                char tmp[30];
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
    uint32_t buffLen = STRING_BUFFER_SIZE;
    executeCommand((uint8_t*)arguments, 0 /*TODO*/, (uint8_t*)STRING_BUFFER, &buffLen, command_mode);
    if(buffLen > 0)
        send(huart, (uint8_t *)STRING_BUFFER, buffLen);
}

void CommandLineParser::send(UART_HandleTypeDef *huart, uint8_t *sendBuffer, uint32_t bufferSize)
{
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
void CommandLineParser::sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize){
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
void CommandLineParser::sendUSB(uint8_t *srcBuffer, uint16_t bufferSize){
    //CDC_Receive_HS(input,1);
    //USBD_CDC_ReceivePacket(&hUsbDeviceHS);
    //flushRead();

    uint8_t result = USBD_OK;
    do
    {
        result = CDC_Transmit_HS(srcBuffer, bufferSize);
        HAL_Delay(10);
    }
    while(result != USBD_OK);
}

/*
 * @brief					tokenizes the arguments from the commands and fills the array 'arguments'
 * @param					the string with the arguments
 */
void CommandLineParser::tokenize_arguments(char* args){
    char* token = strtok(args, " ");
    uint8_t i = 0;
    while(token){
        args[i] = (uint32_t)strtol(token, NULL, 16);
        token = strtok(NULL, " ");
        i++;
    }
}


