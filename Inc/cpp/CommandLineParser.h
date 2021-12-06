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
        GET_VALUES = 0xE,
        NOPE = 0xFF
    } Command; // List of possible commands

    // command help initialization


public:
    CommandLineParser (MemoryController &memoryController);

    void showHelp(uint8_t *inBuff, uint32_t *buffLen);

    MEM_ERROR executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBufflen, Command cmdIdx);

    void executeCommandUART(UART_HandleTypeDef *huart, Command idx);

    void send(UART_HandleTypeDef *huart, uint8_t *sendBuffer, uint32_t bufferSize);

    void sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);

    void sendUSB(uint8_t *srcBuffer, uint16_t bufferSize);

    void tokenize_arguments(char* args);

private:
    Command command_mode = NOPE;
    uint32_t arguments[3];

    uint8_t write_mode = 0xFF;

    uint16_t len;
    uint16_t old_len;

    char Rx_Buffer[100];
    static const char* command_help[];

    static const char * command[];

    MemoryController &m_MemoryController;

    char STRING_BUFFER[STRING_BUFFER_SIZE];

};


#endif //MEMORY_TESTING_FW_COMMANDLINEPARSER_H
