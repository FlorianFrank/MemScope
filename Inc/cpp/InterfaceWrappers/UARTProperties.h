/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_UARTPROPERTIES_H
#define MEMORY_TESTING_FW_UARTPROPERTIES_H


#include "cpp/Devices/DeviceDefines.h"

#if STM32
extern "C" {
#include <stm32f4xx_hal_uart.h>
}
#endif // STM32

namespace UARTProperties
{

/**
 * @brief Enum to select if the interface should be opened in transmit, receive or bidirectional mode.
 */
    enum Mode
    {
        UARTWrapper_TRANSMIT, UARTWrapper_RECEIVE, UARTWrapper_TRANSMIT_RECEIVE
    };

/**
 * @brief Enum to select the word length of the interface.
 */
    enum WordLength
    {
        UARTWrapper_WORD_LENGTH_5,
        UARTWrapper_WORD_LENGTH_6,
        UARTWrapper_WORD_LENGTH_7,
        UARTWrapper_WORD_LENGTH_8,
        UARTWrapper_WORD_LENGTH_9
    };

/**
 * @brief Enum to select the parity of the UART interface.
 */
    enum Parity
    {
        UARTWrapper_NO_PARITY, UARTWrapper_PARITY_EVEN, UARTWrapper_PARITY_ODD
    };

/**
 * @brief Enum to select the stop bits of the UART interface.
 */
    enum UART_StopBits
    {
        UARTWrapper_STOP_BITS_1, UARTWrapper_STOP_BITS_2
    };

#if STM32
    typedef struct __UART_HandleTypeDef UARTInstance;
#endif // STM32

/**
 * @brief This struct contains all properties of an UART interface.
 * It is internally stored in the UARTWrapper class.
 */
    struct UARTHandle
    {
        UARTInstance m_UARTHandle;
        const char *m_InterfaceName;
        uint32_t m_Baudrate;
        Mode m_Mode;
        WordLength m_WordLength;
        Parity m_Parity;
        UART_StopBits m_StopBits;
    };

/**
 * @brief This struct stores value of all
 */
    struct AvailableUARTProperties
    {
#if STM32
        USART_TypeDef *m_UARTHandle{};
#endif
        std::string m_name;
        uint32_t m_minBaudrate{};
        uint32_t m_maxBaudrate{};
        GPIOPin m_rxPin{IO_BANK_UNDEFINED, IO_PIN_UNDEFINED};
        GPIOPin m_txPin{IO_BANK_UNDEFINED, IO_PIN_UNDEFINED};
    };

}

#endif //MEMORY_TESTING_FW_UARTPROPERTIES_H
