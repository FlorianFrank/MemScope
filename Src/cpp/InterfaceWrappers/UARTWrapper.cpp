/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/InterfaceWrappers/UARTWrapper.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal.h"
#include "Logger.h"

#include <string.h> // strcmp
#include <utility> // std::pair
#include <cstring> // memset
#include <iostream>

#define MAX_BUFFER_SIZE 256  // Define a suitable size for your buffer
#define END_OF_TRANSMISSION '\n'  // Special character marking end of transmission
constexpr const char *CONTINUE_STR = "\ncontinue";  // String only used for voltage variations
#define END_JSON '}'  // Special character marking end of transmission


/*static*/ std::map<std::string, UARTWrapper *> UARTWrapper::uartWrapperInstances = {};

UARTWrapper::UARTWrapper(const char *interfaceName, uint32_t baudrate, Mode mode, WordLength wordLen,
                         Parity parity, UART_StopBits stopBits, UART_InterruptMode interruptMode, uint16_t buffering) {
    m_UARTHandle = new UARTHandle;
    m_UARTHandle->m_UARTHandle = new UARTInstance;
    m_UARTHandle->rx_buffer = new uint8_t[256];
    m_UARTHandle->m_InterfaceName = interfaceName;
    m_UARTHandle->m_Baudrate = baudrate;
    m_UARTHandle->m_Mode = mode;
    m_UARTHandle->m_WordLength = wordLen;
    m_UARTHandle->m_Parity = parity;
    m_UARTHandle->m_StopBits = stopBits;
    m_UARTHandle->m_InterruptMode = interruptMode;
    m_UARTHandle->rx_index = 0;
    m_ReceiveCallbackFunction = nullptr;
    m_UARTHandle->m_ReturnBuffer = "";
    m_BufferingSize = buffering;
    m_BufferIdx = 0;
    if (buffering > 0) {
        m_Buffer = new uint8_t[buffering + 2];
        memset(m_Buffer, 0x00, buffering);
    } else
        m_Buffer = nullptr;
#if STM32F429xx
    m_AvailableUARTPorts = {{USART1, "USART1", 9600, 2000000, {IO_BANK_A, IO_PIN_10}, {IO_BANK_A,         IO_PIN_9}},
                            {USART2, "USART2", 9600, 2000000, {IO_BANK_D, IO_PIN_5},  {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
                            {USART3, "USART3", 9600, 2000000, {IO_BANK_D, IO_PIN_5},  {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
                            {UART4,  "UART4",  9600, 2000000, {IO_BANK_C, IO_PIN_11}, {IO_BANK_C,         IO_PIN_10}},
                            {UART5,  "UART5",  9600, 2000000, {IO_BANK_D, IO_PIN_5},  {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
                            {USART6, "USART6", 9600, 2000000, {IO_BANK_D, IO_PIN_5},  {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
                            {UART7,  "UART7",  9600, 2000000, {IO_BANK_D, IO_PIN_5},  {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
                            {UART8,  "UART8",  9600, 2000000, {IO_BANK_D, IO_PIN_5},  {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}}};
#endif // STM32F429xx
}

UARTHandle *UARTWrapper::getUARTHandle() {
    return m_UARTHandle;
}


UARTWrapper::~UARTWrapper() {
    delete m_UARTHandle->rx_buffer;
    delete m_UARTHandle->m_UARTHandle;
    delete m_UARTHandle;
}

/**
 * @brief This function initializes the device specific properties like initializing the GPIO pins, sets the baudrate, stop bits, etc.
 * @return MEM_NO_ERROR if the execution was successful. Otherwise return an error code.
 */
MEM_ERROR UARTWrapper::Initialize() {
    Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Initializing UART interface: %s", m_UARTHandle->m_InterfaceName);
    uartWrapperInstances.insert(
            std::pair<std::string, UARTWrapper *>(std::string(m_UARTHandle->m_InterfaceName), this));
    return InitializeUARTDeviceSpecific(m_UARTHandle);
}

/**
 * @brief This function implements the function to send data over the previously initialized UART interface.
 * @param data here, the data to send must be passed.
 * @param size here the buffer to send should be passed. After executing the function, this variable contains the amount of actually written bytes.
 * @param timeout a timeout must be specified when using blocking functions.
 * @return MEM_ERROR if no error occured otherwise return an error code.
 */
#if STM32

const char *ErrorCodeToString(uint32_t errorCode) {
    switch (errorCode) {
        case HAL_UART_ERROR_NONE:
            return "No Error";
        case HAL_UART_ERROR_PE:
            return "Parity Error";
        case HAL_UART_ERROR_NE:
            return "Noise Error";
        case HAL_UART_ERROR_FE:
            return "Frame Error";
        case HAL_UART_ERROR_ORE:
            return "Overrun Error";
        case HAL_UART_ERROR_DMA:
            return "DMA Transfer Error";
        default:
            return "Unknown Error";
    }
}

MEM_ERROR UARTWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout, bool forceFlush) {
    if (!data || !size)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    int inBuffIdx = 0;
    uint16_t sizeTmp = *size;
    uint16_t bufferFillBefore = m_BufferIdx;

    while (inBuffIdx < sizeTmp) {
        if ((m_BufferingSize - m_BufferIdx) >= (sizeTmp - inBuffIdx)) {
            memcpy(&m_Buffer[m_BufferIdx], &data[inBuffIdx], (sizeTmp - inBuffIdx));
            m_BufferIdx += (sizeTmp - inBuffIdx);
            inBuffIdx += (sizeTmp - inBuffIdx);
        } else {
            int remainingSpace = m_BufferingSize - m_BufferIdx;
            memcpy(&m_Buffer[m_BufferIdx], &data[inBuffIdx], remainingSpace);
            m_BufferIdx = m_BufferingSize;
            inBuffIdx += remainingSpace;
        }

        if (m_BufferIdx == m_BufferingSize || forceFlush) {
            uint16_t sendSize = (m_BufferingSize == 0) ? *size : m_BufferIdx;
            m_Buffer[sendSize] = '\0';
            // Avoid starting next buffer with \n
            if (sendSize - bufferFillBefore == (*size - 1)) {
                sendSize = sendSize + 2;
                m_Buffer[sendSize - 2] = '\n';
                m_Buffer[sendSize - 1] = '\0';
            } else if (sendSize - bufferFillBefore < (*size - 1)) {
                sendSize = sendSize + 1;
                m_Buffer[sendSize - 1] = '\0';
            }
            auto ret = HAL_UART_Transmit(m_UARTHandle->m_UARTHandle, m_Buffer, sendSize, timeout);
            // Start filling the buffer from the beginning starting at the next loop iteration
            m_BufferIdx = 0;
            memset(m_Buffer, 0x00, m_BufferingSize);

            if (ret == HAL_BUSY) {
                Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "Interface %s is busy",
                            m_UARTHandle->m_InterfaceName);
                return MemoryErrorHandling::MEM_HAL_BUSY;
            }

            if (ret == HAL_ERROR) {
                UART_HandleTypeDef *handle = reinterpret_cast<UART_HandleTypeDef *>(m_UARTHandle->m_UARTHandle);
                uint32_t errorCode = handle->ErrorCode;
                Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__,
                            "Interface %s returned with an error: %s (0x%08lX)",
                            m_UARTHandle->m_InterfaceName, ErrorCodeToString(errorCode), errorCode);
                return MemoryErrorHandling::MEM_IO_ERROR;
            }

            if (ret == HAL_TIMEOUT) {
                Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "Interface %s caused a timeout",
                            m_UARTHandle->m_InterfaceName);
                return MemoryErrorHandling::MEM_HAL_TIMEOUT;
            }
            // actually sent size can be higher then currently passed data when there is data in the buffer remaining
            *size += sendSize;
        }
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

#else
MEM_ERROR UARTWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout, bool forceFlush) {
    return MemoryErrorHandling::MEM_INTERFACE_NOT_FOUND;
}
#endif

/**
 * @brief This function implements the function to receive data from the previously initialized UART interface.
 * @param data the buffer to receive the data. It must be equally or larger the size specified in size.
 * @param size here the buffer to receive should be passed. After executing the function, this variable contains the amount of actually received bytes.
 * @param timeout a timeout must be specified when using blocking functions.
 * @return MEM_ERROR if no error occured otherwise return an error code.
 */
#if STM32

MEM_ERROR UARTWrapper::ReceiveData(uint8_t *data, uint16_t *size, BlockingMode blockingMode, uint32_t timeout) {
    HAL_StatusTypeDef ret;
    if (blockingMode == BLOCKING) {
        ret = HAL_UART_Receive(m_UARTHandle->m_UARTHandle, data, *size, timeout);
    } else {
        Logger::log(LogLevel::DEBUG, __FILE__, __LINE__,
                    "Entering non-blocking mode (HAL_UART_Receive_IT) for UART receive");
        memset(m_UARTHandle->rx_buffer, 0x00, 256);
        ret = HAL_UART_Receive_IT(m_UARTHandle->m_UARTHandle, m_UARTHandle->rx_buffer, 512);
    }

    if (ret == HAL_BUSY) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART Receive is busy. Unable to process request.");
        return MemoryErrorHandling::MEM_HAL_BUSY;
    }
    if (ret == HAL_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART Receive encountered an I/O error.");
        return MemoryErrorHandling::MEM_IO_ERROR;
    }
    if (ret == HAL_TIMEOUT) {
        Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "UART Receive operation timed out.");
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;
    }
    if (blockingMode == BLOCKING)
        Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Data received successfully, size: %d bytes.", *size);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

#else
MEM_ERROR UARTWrapper::ReceiveData(uint8_t *data, uint16_t *size, BlockingMode blockingMode, uint32_t timeout) {
    return MemoryErrorHandling::MEM_INTERFACE_NOT_FOUND;
}
#endif

#if STM32

/**
 * @brief This function initializes the device specific properties of an UART interface.
 * In this case the function is implementd for an STM32 board. It sets the baudrate, word length, stop bits, etc.
 */
MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTHandle *uartProperties) {
    Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Initialize UART on STM32F429 Interface: %s Baudrate: %d",
                m_UARTHandle->m_InterfaceName, m_UARTHandle->m_Baudrate);
    int elemCtr = 0;
    bool interfaceFound = false;
    for (const AvailableUARTProperties &availPorts: m_AvailableUARTPorts) {
        if (availPorts.GetName() == uartProperties->m_InterfaceName) {
            uartProperties->m_UARTHandle->Instance = m_AvailableUARTPorts[elemCtr].GetUARTHandle();
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Interface %s detected", m_UARTHandle->m_InterfaceName);
            interfaceFound = true;
            if (uartProperties->m_Baudrate < availPorts.GetMinBaudrate() ||
                uartProperties->m_Baudrate > availPorts.GetMaxBaudrate()) {
                Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Baudrate %d not supported", m_UARTHandle->m_Baudrate);
                return MemoryErrorHandling::MEM_UNSUPPORTED_BAUDRATE;
            }

            break;
        }
        elemCtr++;
    }

    if (!interfaceFound) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Interface %s not found", m_UARTHandle->m_InterfaceName);
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
    }


    uartProperties->m_UARTHandle->Init.BaudRate = uartProperties->m_Baudrate;

    if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_5 ||
        uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_6 ||
        uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_7)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;
    else if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_8)
        uartProperties->m_UARTHandle->Init.WordLength = UART_WORDLENGTH_8B;
    else if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_9)
        uartProperties->m_UARTHandle->Init.WordLength = UART_WORDLENGTH_9B;

    if (uartProperties->m_StopBits == UARTWrapper_STOP_BITS_1)
        uartProperties->m_UARTHandle->Init.StopBits = UART_STOPBITS_1;
    else if (uartProperties->m_StopBits == UARTWrapper_STOP_BITS_2)
        uartProperties->m_UARTHandle->Init.StopBits = UART_STOPBITS_2;

    if (uartProperties->m_Parity == UARTWrapper_NO_PARITY)
        uartProperties->m_UARTHandle->Init.Parity = UART_PARITY_NONE;
    else if (uartProperties->m_Parity == UARTWrapper_PARITY_EVEN)
        uartProperties->m_UARTHandle->Init.Parity = UART_PARITY_EVEN;
    else if (uartProperties->m_Parity == UARTWrapper_PARITY_ODD)
        uartProperties->m_UARTHandle->Init.Parity = UART_PARITY_ODD;

    if (uartProperties->m_Mode == UARTWrapper_TRANSMIT)
        uartProperties->m_UARTHandle->Init.Mode = UART_MODE_TX;
    if (uartProperties->m_Mode == UARTWrapper_RECEIVE)
        uartProperties->m_UARTHandle->Init.Mode = UART_MODE_RX;
    if (uartProperties->m_Mode == UARTWrapper_TRANSMIT_RECEIVE)
        uartProperties->m_UARTHandle->Init.Mode = UART_MODE_TX_RX;

    uartProperties->m_UARTHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartProperties->m_UARTHandle->Init.OverSampling = UART_OVERSAMPLING_16;
    uartProperties->m_UARTHandle->gState = HAL_UART_STATE_RESET;
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Call HAL_UART_Init");
    if (HAL_UART_Init(uartProperties->m_UARTHandle) != HAL_OK) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "HAL_UART_Init failed!");
        return MemoryErrorHandling::MEM_HAL_INTERNAL_ERROR;
    }

    if (m_UARTHandle->m_InterruptMode == UART_MODE_INTERRUPT) {
        Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Initializing interrupt mode for interface: %s",
                    m_UARTHandle->m_InterfaceName);

        __HAL_UART_ENABLE_IT(m_UARTHandle->m_UARTHandle, UART_IT_RXNE);


        if (strcmp(m_UARTHandle->m_InterfaceName, "USART1") == 0) {
            HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART1_IRQn);
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "USART1 IRQ enabled");
        } else if (strcmp(m_UARTHandle->m_InterfaceName, "USART2") == 0) {
            HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART2_IRQn);
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "USART2 IRQ enabled");
        } else if (strcmp(m_UARTHandle->m_InterfaceName, "USART3") == 0) {
            HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART3_IRQn);
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "USART3 IRQ enabled");
        } else {
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "Unknown interface: %s", m_UARTHandle->m_InterfaceName);
        }
    }


    Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Interface %s initialized", m_UARTHandle->m_InterfaceName);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

void UARTWrapper::SendData(uint8_t *msg, uint16_t msg_len, uint32_t timeout, bool forceFlush) {
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Send data: %s", msg);

    // Transmit the data over UART
    auto returnValue = HAL_UART_Transmit(m_UARTHandle->m_UARTHandle, msg, msg_len, timeout);
    switch (returnValue) {
        case HAL_BUSY:
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART Transmit is busy. Unable to process request.");
            break;
        case HAL_ERROR:
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART Transmit encountered an I/O error.");
            break;
        case HAL_TIMEOUT:
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "UART Transmit operation timed out.");

    }
}

vector<uint8_t> UARTWrapper::ReceiveToIdle(uint16_t size, uint32_t timeout) {
    uint8_t data[MAX_BUFFER_SIZE];
    uint16_t real_size;
    Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Waiting for data to receive");
    switch (HAL_UARTEx_ReceiveToIdle(m_UARTHandle->m_UARTHandle, data, size, &real_size, timeout)) {
        case HAL_OK:
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Received %d of data ", real_size);
            break;
        case HAL_ERROR:
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "HAL_UARTEx_ReceiveToIdle returned an error");
            break;
        case HAL_BUSY:
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "Interface busy");
            break;
        case HAL_TIMEOUT:
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "Interface timeout occured");
            break;
    }
    vector<uint8_t> r(data, data + real_size);
    return r;
}

#else
/**
 * @brief Dummy implementation to allow compilation when no board is defined.
 */
MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTHandle *uartHandle) {
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}
#endif // STM32

#define STM32 1
#if STM32

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Call HAL_UART_RxCpltCallback");
    if (huart->Instance == USART1) {
        Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Instance USART1 detected");
        // Process received data
        auto it = UARTWrapper::uartWrapperInstances.find("USART1");
        if (it != UARTWrapper::uartWrapperInstances.end()) {
            //memset(it->second->getUARTHandle()->rx_buffer, 0x00, 512);
            HAL_StatusTypeDef status = HAL_UART_Receive_IT(it->second->getUARTHandle()->m_UARTHandle,
                                                           it->second->getUARTHandle()->rx_buffer, 256);
            if (status == HAL_OK) {
                Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Received data %s",
                            it->second->getUARTHandle()->rx_buffer);
            } else {
                Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Error re-enabling UART receive interrupt: %d",
                            status);
            }
        } else {
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "USART1 not found in the map.");
        }
    } else {
        Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "USART1 not found in the map.");
    }
}

bool second = false;

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART Error detected\n");

    if (huart->Instance == USART1) {
        if (huart->ErrorCode & HAL_UART_ERROR_PE) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Parity Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_NE) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Noise Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_FE) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Frame Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_ORE) {
            __HAL_UART_CLEAR_OREFLAG(huart);
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Overrun Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_DMA) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "DMA Transfer Error\n");
        }
        if (huart->ErrorCode == HAL_UART_ERROR_NONE) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "No specific error detected\n");
        }
    }
    auto it = UARTWrapper::uartWrapperInstances.find("USART1");
    if (it != UARTWrapper::uartWrapperInstances.end()) {
        //memset(it->second->getUARTHandle()->rx_buffer, 0x00, 512);
        HAL_StatusTypeDef status = HAL_UART_Receive_IT(huart, it->second->getUARTHandle()->rx_buffer, 256);
        if (status != HAL_OK) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to restart UART reception: %d", status);
        }
        Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "DATA: %s", it->second->getUARTHandle()->rx_buffer);
    }
}

extern "C" void USART1_IRQHandler(void) {
    auto it = UARTWrapper::uartWrapperInstances.find("USART1");
    if (it == UARTWrapper::uartWrapperInstances.end()) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "USART1 instance not found.");
        return;
    }

    auto uartHandle = it->second->getUARTHandle();
    if (!uartHandle) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART handle is null.");
        return;
    }

    auto rx_buffer = uartHandle->rx_buffer;
    auto &returnBuffer = uartHandle->m_ReturnBuffer;

    if (USART1->SR & USART_SR_RXNE) {
        char received_char = USART1->DR;

        if (it->second->m_ReceiveCallbackCalled) {
            it->second->m_ReceiveCallbackCalled = false;
            returnBuffer.clear();
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Callback reset. Return buffer cleared.");
        }
        if (uartHandle->rx_index == 1 && received_char == '"') {
            Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "Ignoring leading double quote.");
        } else {
            if (received_char == END_OF_TRANSMISSION) {
                if (uartHandle->rx_index < MAX_BUFFER_SIZE) {
                    rx_buffer[uartHandle->rx_index] = '\0';
                    returnBuffer += reinterpret_cast<char *>(rx_buffer);
            //        if (second)
              //          printf("SACKL %s\n", returnBuffer.c_str());
                    if (strcmp(CONTINUE_STR, returnBuffer.c_str()) == 0 || strcmp("continue", returnBuffer.c_str()) == 0) { // TODO greislig
                        Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Received continue string");
                        it->second->m_ReceiveCallbackFunction(returnBuffer, MSG_CONTINUE);
                        returnBuffer.clear();
                        second = true;
                    } else {
                        returnBuffer += "\n";
                    }


                    uartHandle->rx_index = 0;
                } else {
                    Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "RX buffer overflow at END_OF_TRANSMISSION.");
                }
            } else if (received_char == END_JSON) {
                returnBuffer += "}\n";
                // Logger::log(LogLevel::INFO,__FILE__, __LINE__, "JSON received: %s", returnBuffer.c_str());
                it->second->m_ReceiveCallbackFunction(returnBuffer, MSG_CMD);
                it->second->m_ReceiveCallbackCalled = true;
            } else {
                if (uartHandle->rx_index < MAX_BUFFER_SIZE - 1) {
                    rx_buffer[uartHandle->rx_index++] = received_char;
                } else {
                    Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "RX buffer overflow. Index reset.");
                    uartHandle->rx_index = 0;
                }
            }
        }
    } else {
        Logger::log(LogLevel::DEBUG, __FILE__, __LINE__, "No data to read from USART1.");
    }
}


void UARTWrapper::RegisterReceiveCallback(void (*receiveCallback)(std::string &str, RecvMsgType receivedMsgType)) {
    m_ReceiveCallbackFunction = receiveCallback;
}

#else
MEM_ERROR UARTWrapper::RegisterReceiveCallback(void (*receiveCallback)(*)){
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}
#endif