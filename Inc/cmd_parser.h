//
// Created by florianfrank on 01.07.21.
//

#ifndef MEMORY_TESTING_FW_CMD_PARSER_H
#define MEMORY_TESTING_FW_CMD_PARSER_H

#include "memory_error_handling.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"

uint8_t write_mode;


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

void showHelp(uint8_t *inBuff, uint32_t *buffLen);

MEM_ERROR executeCommand(uint8_t *inBuff, uint32_t *inBuffLen, uint8_t *outBuff, uint32_t *outBufflen, Command cmdIdx);

extern void executeCommandUART(UART_HandleTypeDef *huart, Command idx);

#include "memory_control.h"
#include "usbd_cdc_if.h"
#include "memory_defines.h"

#endif //MEMORY_TESTING_FW_CMD_PARSER_H
