/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_UARTPROPERTIES_H
#define MEMORY_TESTING_FW_UARTPROPERTIES_H
#include "cpp/Devices/DeviceDefines.h"

#include <string> // std::string
#include <utility>

#if STM32
extern "C" {
#include "stm32f4xx_hal_dma.h"
#include <stm32f4xx_hal_uart.h>
}
typedef USART_TypeDef* UARTInnerInstance;
#else
typedef int UARTInnerInstance;
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

    enum UART_InterruptMode {
        UART_MODE_BLOCKING, UART_MODE_INTERRUPT, UART_MODE_DMA
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
        UARTWrapper_NO_PARITY,
        UARTWrapper_PARITY_EVEN,
        UARTWrapper_PARITY_ODD
    };

    /**
     * @brief Enum to select the stop bits of the UART interface.
     */
    enum UART_StopBits
    {
        UARTWrapper_STOP_BITS_1,
        UARTWrapper_STOP_BITS_2
    };

#if STM32
    typedef UART_HandleTypeDef UARTInstance;
#else
    typedef uint32_t UARTInstance;
#endif // STM32

    /**
     * @brief This struct contains all properties of an UART interface.
     * It is internally stored in the UARTWrapper class.
     */
    struct UARTHandle
    {
        UARTInstance* m_UARTHandle;
        const char *m_InterfaceName;
        uint32_t m_Baudrate;
        Mode m_Mode;
        WordLength m_WordLength;
        Parity m_Parity;
        UART_StopBits m_StopBits;
        UART_InterruptMode m_InterruptMode;
        uint8_t* rx_buffer;
        uint32_t rx_index;
        std::string m_ReturnBuffer;
    };

    /**
     * @brief This struct stores value of all
     */
    class AvailableUARTProperties
    {
    public:
        AvailableUARTProperties(UARTInnerInstance uartHandle, std::string name, uint32_t minBaudrate,
                                uint32_t maxBaudrate, const GPIOPin &rxPin, const GPIOPin &txPin) : m_UARTHandle(
                uartHandle), m_name(std::move(name)), m_minBaudrate(minBaudrate), m_maxBaudrate(maxBaudrate), m_rxPin(rxPin),
                                                                                                    m_txPin(txPin)
        {}

        UARTInnerInstance GetUARTHandle() const
        {
            return m_UARTHandle;
        }

        const std::string &GetName() const
        {
            return m_name;
        }

        uint32_t GetMinBaudrate() const
        {
            return m_minBaudrate;
        }

        uint32_t GetMaxBaudrate() const
        {
            return m_maxBaudrate;
        }

        const GPIOPin &GetRxPin() const
        {
            return m_rxPin;
        }

        const GPIOPin &GetTxPin() const
        {
            return m_txPin;
        }

    private:

        UARTInnerInstance m_UARTHandle{};
        std::string m_name;
        uint32_t m_minBaudrate{};
        uint32_t m_maxBaudrate{};
        GPIOPin m_rxPin{IO_BANK_UNDEFINED, IO_PIN_UNDEFINED};
        GPIOPin m_txPin{IO_BANK_UNDEFINED, IO_PIN_UNDEFINED};
    };

}
#endif //MEMORY_TESTING_FW_UARTPROPERTIES_H
