/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/InterfaceWrappers/UARTWrapper.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal.h"
#include "Logger.h"

#include <string.h>
#include "string.h" // strcmp
#include <utility> // std::pair
#include <cstring> // memset

#define MAX_BUFFER_SIZE 256  // Define a suitable size for your buffer
#define END_OF_TRANSMISSION '\n'  // Special character marking end of transmission
#define END_JSON '}'  // Special character marking end of transmission


/*static*/ std::map<std::string, UARTWrapper*> UARTWrapper::uartWrapperInstances = {};

UARTWrapper::UARTWrapper(const char *interfaceName, uint32_t baudrate, Mode mode, WordLength wordLen,
                         Parity parity, UART_StopBits stopBits, UART_InterruptMode interruptMode) {
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

UARTHandle* UARTWrapper::getUARTHandle() {
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
    Logger::log(LogLevel::DEBUG, "Initializing UART interface: %s", m_UARTHandle->m_InterfaceName);
    uartWrapperInstances.insert(std::pair<std::string,UARTWrapper*>(std::string(m_UARTHandle->m_InterfaceName),this));
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
MEM_ERROR UARTWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    if(!data || !size)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    HAL_StatusTypeDef ret = HAL_UART_Transmit(reinterpret_cast<UART_HandleTypeDef*>(m_UARTHandle->m_UARTHandle), data, *size, timeout);
    if(ret == HAL_BUSY)
        return MemoryErrorHandling::MEM_HAL_BUSY;
    if (ret == HAL_ERROR)
        return MemoryErrorHandling::MEM_IO_ERROR;
    if (ret == HAL_TIMEOUT)
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;

    return MemoryErrorHandling::MEM_NO_ERROR;
}
#else
MEM_ERROR UARTWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout) {
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
    if(blockingMode == BLOCKING) {
        ret = HAL_UART_Receive(m_UARTHandle->m_UARTHandle, data, *size, timeout);
    } else {
        Logger::log(LogLevel::DEBUG, "Entering non-blocking mode (HAL_UART_Receive_IT) for UART receive");
        memset(m_UARTHandle->rx_buffer, 0x00, 256);
        ret = HAL_UART_Receive_IT(m_UARTHandle->m_UARTHandle, m_UARTHandle->rx_buffer, 512);
    }

    if (ret == HAL_BUSY) {
        Logger::log(LogLevel::ERROR, "UART Receive is busy. Unable to process request.");
        return MemoryErrorHandling::MEM_HAL_BUSY;
    }
    if (ret == HAL_ERROR) {
        Logger::log(LogLevel::ERROR, "UART Receive encountered an I/O error.");
        return MemoryErrorHandling::MEM_IO_ERROR;
    }
    if (ret == HAL_TIMEOUT) {
        Logger::log(LogLevel::WARNING, "UART Receive operation timed out.");
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;
    }
    if(blockingMode == BLOCKING)
        Logger::log(LogLevel::INFO, "Data received successfully, size: %d bytes.", *size);
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
MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTHandle *uartProperties)
{
    Logger::log(LogLevel::DEBUG, "Initialize UART on STM32F429 Interface: %s Baudrate: %d",
                m_UARTHandle->m_InterfaceName,  m_UARTHandle->m_Baudrate);
    int elemCtr = 0;
    bool interfaceFound = false;
    for(const AvailableUARTProperties& availPorts: m_AvailableUARTPorts)
    {
        if(availPorts.GetName() == uartProperties->m_InterfaceName)
        {
            uartProperties->m_UARTHandle->Instance = m_AvailableUARTPorts[elemCtr].GetUARTHandle();
            Logger::log(LogLevel::DEBUG, "Interface %s detected", m_UARTHandle->m_InterfaceName);
            interfaceFound = true;
            if (uartProperties->m_Baudrate < availPorts.GetMinBaudrate() ||
                uartProperties->m_Baudrate > availPorts.GetMaxBaudrate()) {
                Logger::log(LogLevel::ERROR, "Baudrate %d not supported", m_UARTHandle->m_Baudrate);
                return MemoryErrorHandling::MEM_UNSUPPORTED_BAUDRATE;
            }

            break;
        }
        elemCtr++;
    }

    if (!interfaceFound) {
        Logger::log(LogLevel::ERROR, "Interface %s not found", m_UARTHandle->m_InterfaceName);
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
    else if(uartProperties->m_StopBits == UARTWrapper_STOP_BITS_2)
        uartProperties->m_UARTHandle->Init.StopBits = UART_STOPBITS_2;

    if(uartProperties->m_Parity == UARTWrapper_NO_PARITY)
        uartProperties->m_UARTHandle->Init.Parity = UART_PARITY_NONE;
    else if(uartProperties->m_Parity == UARTWrapper_PARITY_EVEN)
        uartProperties->m_UARTHandle->Init.Parity = UART_PARITY_EVEN;
    else if(uartProperties->m_Parity == UARTWrapper_PARITY_ODD)
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
    Logger::log(LogLevel::INFO, "Call HAL_UART_Init");
    if (HAL_UART_Init(uartProperties->m_UARTHandle) != HAL_OK) {
        Logger::log(LogLevel::ERROR, "HAL_UART_Init failed!");
        return MemoryErrorHandling::MEM_HAL_INTERNAL_ERROR;
    }

    if (m_UARTHandle->m_InterruptMode == UART_MODE_INTERRUPT) {
        Logger::log(LogLevel::DEBUG, "Initializing interrupt mode for interface: %s", m_UARTHandle->m_InterfaceName);

        __HAL_UART_ENABLE_IT(m_UARTHandle->m_UARTHandle, UART_IT_RXNE);


        if (strcmp(m_UARTHandle->m_InterfaceName, "USART1") == 0) {
            HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART1_IRQn);
            Logger::log(LogLevel::DEBUG, "USART1 IRQ enabled");
        }
        else if (strcmp(m_UARTHandle->m_InterfaceName, "USART2") == 0) {
            HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART2_IRQn);
            Logger::log(LogLevel::DEBUG, "USART2 IRQ enabled");
        }
        else if (strcmp(m_UARTHandle->m_InterfaceName, "USART3") == 0) {
            HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART3_IRQn);
            Logger::log(LogLevel::DEBUG, "USART3 IRQ enabled");
        }
        else {
            Logger::log(LogLevel::WARNING, "Unknown interface: %s", m_UARTHandle->m_InterfaceName);
        }
    }


    Logger::log(LogLevel::DEBUG, "Interface %s initialized", m_UARTHandle->m_InterfaceName);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

void UARTWrapper::SendData(uint8_t* msg, uint16_t msg_len, uint32_t timeout) {
    Logger::log(LogLevel::INFO, "Send data: %s", msg);

    // Transmit the data over UART
    auto returnValue = HAL_UART_Transmit(m_UARTHandle->m_UARTHandle, msg, msg_len, timeout);
    switch(returnValue)
    {
        case HAL_BUSY:
            Logger::log(LogLevel::ERROR, "UART Transmit is busy. Unable to process request.");
            break;
        case HAL_ERROR:
            Logger::log(LogLevel::ERROR, "UART Transmit encountered an I/O error.");
            break;
        case HAL_TIMEOUT:
            Logger::log(LogLevel::WARNING, "UART Transmit operation timed out.");

    }
}

vector<uint8_t> UARTWrapper::ReceiveToIdle(uint16_t size, uint32_t timeout) {
  // TODO: timeout
  uint8_t data[256] ;// TODO size
  uint16_t real_size;
    Logger::log(LogLevel::DEBUG, "Waiting for data to receive");
    switch (HAL_UARTEx_ReceiveToIdle(m_UARTHandle->m_UARTHandle, data, size, &real_size, timeout)) {
        case HAL_OK:
            Logger::log(LogLevel::DEBUG, "Received %d of data ", real_size);
            break;
        case HAL_ERROR:
            Logger::log(LogLevel::ERROR, "HAL_UARTEx_ReceiveToIdle returned an error");
            break;
        case HAL_BUSY:
            Logger::log(LogLevel::WARNING, "Interface busy");
            break;
        case HAL_TIMEOUT:
            Logger::log(LogLevel::WARNING, "Interface timeout occured");
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
    Logger::log(LogLevel::DEBUG, "Call HAL_UART_RxCpltCallback");
    if (huart->Instance == USART1) {
        Logger::log(LogLevel::DEBUG, "Instance USART1 detected");
        // Process received data
        auto it = UARTWrapper::uartWrapperInstances.find("USART1");
        if (it != UARTWrapper::uartWrapperInstances.end()) {
            //memset(it->second->getUARTHandle()->rx_buffer, 0x00, 512);
            HAL_StatusTypeDef status = HAL_UART_Receive_IT(it->second->getUARTHandle()->m_UARTHandle, it->second->getUARTHandle()->rx_buffer, 256);
            if (status == HAL_OK) {
                Logger::log(LogLevel::DEBUG, "Received data %s", it->second->getUARTHandle()->rx_buffer);
            } else {
                Logger::log(LogLevel::ERROR, "Error re-enabling UART receive interrupt: %d", status);
            }
        } else {
            Logger::log(LogLevel::WARNING, "USART1 not found in the map.");
        }
    } else {
        Logger::log(LogLevel::WARNING, "USART1 not found in the map.");
    }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    printf("UART Error detected\n");

    if (huart->Instance == USART1) {
        if (huart->ErrorCode & HAL_UART_ERROR_PE) {
            printf("Parity Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_NE) {
            printf("Noise Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_FE) {
            printf("Frame Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_ORE) {
            __HAL_UART_CLEAR_OREFLAG(huart);
            printf("Overrun Error\n");
        }
        if (huart->ErrorCode & HAL_UART_ERROR_DMA) {
            printf("DMA Transfer Error\n");
        }
        if (huart->ErrorCode == HAL_UART_ERROR_NONE) {
            printf("No specific error detected\n");
        }
    }
    auto it = UARTWrapper::uartWrapperInstances.find("USART1");
    if (it != UARTWrapper::uartWrapperInstances.end()) {
        //memset(it->second->getUARTHandle()->rx_buffer, 0x00, 512);
        HAL_StatusTypeDef status = HAL_UART_Receive_IT(huart, it->second->getUARTHandle()->rx_buffer, 256);
        if (status != HAL_OK) {
            Logger::log(LogLevel::ERROR, "Failed to restart UART reception: %d", status);
        }
        Logger::log(LogLevel::DEBUG, "DATA: %s", it->second->getUARTHandle()->rx_buffer);
    }
}

extern "C" void USART1_IRQHandler(void) {
    auto it = UARTWrapper::uartWrapperInstances.find("USART1");
    if (it == UARTWrapper::uartWrapperInstances.end()) {
        Logger::log(LogLevel::ERROR, "USART1 instance not found.");
        return;
    }

    auto uartHandle = it->second->getUARTHandle();
    if (!uartHandle) {
        Logger::log(LogLevel::ERROR, "UART handle is null.");
        return;
    }

    auto rx_buffer = uartHandle->rx_buffer;
    auto &returnBuffer = uartHandle->m_ReturnBuffer;

    if (USART1->SR & USART_SR_RXNE) {
        char received_char = USART1->DR;

        if (it->second->m_ReceiveCallbackCalled) {
            it->second->m_ReceiveCallbackCalled = false;
            returnBuffer.clear();
            Logger::log(LogLevel::DEBUG, "Callback reset. Return buffer cleared.");
        }

        if (uartHandle->rx_index == 1 && received_char == '"') {
            Logger::log(LogLevel::DEBUG, "Ignoring leading double quote.");
        } else {
            if (received_char == END_OF_TRANSMISSION) {
                if (uartHandle->rx_index < MAX_BUFFER_SIZE) {
                    rx_buffer[uartHandle->rx_index] = '\0';
                    returnBuffer += reinterpret_cast<char *>(rx_buffer);
                    returnBuffer += "\n";
                    uartHandle->rx_index = 0;
                } else {
                    Logger::log(LogLevel::WARNING, "RX buffer overflow at END_OF_TRANSMISSION.");
                }
            } else if (received_char == END_JSON) {
                returnBuffer += "}\n";
                Logger::log(LogLevel::INFO, "JSON received: %s", returnBuffer.c_str());
                it->second->m_ReceiveCallbackFunction(returnBuffer);
                it->second->m_ReceiveCallbackCalled = true;
            } else {
                if (uartHandle->rx_index < MAX_BUFFER_SIZE - 1) {
                    rx_buffer[uartHandle->rx_index++] = received_char;
                } else {
                    Logger::log(LogLevel::WARNING, "RX buffer overflow. Index reset.");
                    uartHandle->rx_index = 0;
                }
            }
        }
    } else {
        Logger::log(LogLevel::DEBUG, "No data to read from USART1.");
    }
}


void UARTWrapper::RegisterReceiveCallback(void (*receiveCallback)(std::string &str)) {
    m_ReceiveCallbackFunction = receiveCallback;
}
#else
MEM_ERROR UARTWrapper::RegisterReceiveCallback(void (*receiveCallback)(*)){
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}
#endif