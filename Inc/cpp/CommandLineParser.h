//
// Created by florianfrank on 03.12.21.
//

#ifndef MEMORY_TESTING_FW_COMMANDLINEPARSER_H
#define MEMORY_TESTING_FW_COMMANDLINEPARSER_H


#include <memory_error_handling.h>
#include <cstdint>
#include <stm32f4xx_hal_uart.h>

class CommandLineParser
{
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

public:
    void showHelp(uint8_t *inBuff, uint32_t *buffLen);

    MEM_ERROR executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBufflen, Command cmdIdx);

    void executeCommandUART(UART_HandleTypeDef *huart, Command idx);

    void send(UART_HandleTypeDef *huart, uint8_t *sendBuffer, uint32_t bufferSize);

    void sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);

    void sendUSB(uint8_t *srcBuffer, uint16_t bufferSize);

    void tokenize_arguments(char* args);

private:
    uint8_t command_mode = 0xFF;
    uint32_t arguments[3];

    uint8_t write_mode = 0xFF;

    uint16_t len;
    uint16_t old_len;

    char Rx_Buffer[100];


};


#endif //MEMORY_TESTING_FW_COMMANDLINEPARSER_H
