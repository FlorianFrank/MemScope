//
// Created by florianfrank on 14.12.21.
//

#ifndef MEMORY_TESTING_FW_UARTPROPERTIES_H
#define MEMORY_TESTING_FW_UARTPROPERTIES_H

#if STM32
extern "C" {
#include <stm32f4xx_hal_uart.h>
};

enum UART_Mode {
    UARTWrapper_TRANSMIT,
    UARTWrapper_RECEIVE,
    UARTWrapper_TRANSMIT_RECEIVE
};


enum UART_WordLength {
    UARTWrapper_WORD_LENGTH_5,
    UARTWrapper_WORD_LENGTH_6,
    UARTWrapper_WORD_LENGTH_7,
    UARTWrapper_WORD_LENGTH_8,
    UARTWrapper_WORD_LENGTH_9
};

enum UART_Partiy {
    UARTWrapper_NO_PARITY,
    UARTWrapper_PARITY_EVEN,
    UARTWrapper_PARITY_ODD
};

enum UART_StopBits {
    UARTWrapper_STOP_BITS_1,
    UARTWrapper_STOP_BITS_2
};

#if STM32
struct __UART_HandleTypeDef;
typedef struct __UART_HandleTypeDef UARTHandle;
#endif // STM32

struct UARTProperties {
    UARTHandle m_UARTHandle;
    const char* m_InterfaceName;
    uint32_t m_Baudrate;
    UART_Mode m_Mode;
    UART_WordLength m_WordLength;
    UART_Partiy m_Parity;
    UART_StopBits m_StopBits;
};

struct AvailableUARTProperties {
#if STM32
    USART_TypeDef* m_UARTHandle;
#endif
    const char m_name[10];
    uint32_t m_minBaudrate;
    uint32_t m_maxBaudrate;
    const char m_rxPin[4];
    const char m_txPin[4];
};



#endif //MEMORY_TESTING_FW_UARTPROPERTIES_H
