//
// Created by florianfrank on 03.12.21.
//

#ifndef MEMORY_TESTING_FW_COMMANDLINEPARSER_H
#define MEMORY_TESTING_FW_COMMANDLINEPARSER_H


#include "cpp/MemoryController.h"
#include "CommandDefines.h"
#include <cstdint>

extern "C" {
#include <stm32f4xx_hal_uart.h>
};

class CommandLineParser
{
public:

    explicit CommandLineParser (MemoryController *memoryController, InterfaceWrappers *interface);

    void showHelp(uint8_t *inBuff, uint32_t *buffLen);

    MEM_ERROR executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBufflen, Command cmdIdx);


    void send(UART_HandleTypeDef *huart, uint8_t *sendBuffer, uint32_t bufferSize);

    void executeCommandUART(UART_HandleTypeDef *huart, Command idx);

    void sendUART(UART_HandleTypeDef *huart, uint8_t *srcBuffer, uint32_t bufferSize);

    void sendUSB(uint8_t *srcBuffer, uint16_t bufferSize);

    void tokenize_arguments(char* args);

private:
    CommandLineParser()= default;
    Command m_commandMode = NOPE;
    uint32_t m_arguments[3];
    uint8_t m_WriteMode = 0xFF;
    uint16_t len;
    uint16_t old_len;
    char Rx_Buffer[100];
    static const char* command_help[];
    static const char * command[];
    MemoryController *m_MemoryController;
    char STRING_BUFFER[STRING_BUFFER_SIZE];

    InterfaceWrappers *m_InterfaceWrapper;

};


#endif //MEMORY_TESTING_FW_COMMANDLINEPARSER_H
