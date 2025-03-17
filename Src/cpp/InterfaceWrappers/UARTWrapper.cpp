/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/InterfaceWrappers/UARTWrapper.h"
#include "Logger.h"
UARTWrapper::UARTWrapper(const char* interfaceName, uint32_t baudrate, Mode mode, WordLength wordLen,
                                                  Parity parity, UART_StopBits stopBits)
{
    m_UARTHandle = new UARTHandle;
    m_UARTHandle->m_InterfaceName = interfaceName;
    m_UARTHandle->m_Baudrate = baudrate;
    m_UARTHandle->m_Mode = mode;
    m_UARTHandle->m_WordLength = wordLen;
    m_UARTHandle->m_Parity = parity;
    m_UARTHandle->m_StopBits = stopBits;

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


UARTWrapper::~UARTWrapper()
{
    delete m_UARTHandle;
}

 /**
  * @brief This function initializes the device specific properties like initializing the GPIO pins, sets the baudrate, stop bits, etc.
  * @return MEM_NO_ERROR if the execution was successful. Otherwise return an error code.
  */
MEM_ERROR UARTWrapper::Initialize()
{
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

    HAL_StatusTypeDef ret = HAL_UART_Transmit(&m_UARTHandle->m_UARTHandle, data, *size, timeout);
    if(ret == HAL_BUSY)
        return MemoryErrorHandling::MEM_HAL_BUSY;
    if (ret == HAL_ERROR)
        return MemoryErrorHandling::MEM_IO_ERROR;
    if (ret == HAL_TIMEOUT)
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;

    return MemoryErrorHandling::MEM_NO_ERROR;
}
#else
MEM_ERROR UARTWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
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
MEM_ERROR UARTWrapper::ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    HAL_StatusTypeDef ret = HAL_UART_Receive(&m_UARTHandle->m_UARTHandle, data, *size, timeout);
    if(ret == HAL_BUSY){
        Logger::log(LogLevel::ERROR, "UART Receive is busy. Unable to process request.");
        return MemoryErrorHandling::MEM_HAL_BUSY;
    }
    if (ret == HAL_ERROR){
        Logger::log(LogLevel::ERROR, "UART Receive encountered an I/O error.");
        return MemoryErrorHandling::MEM_IO_ERROR;
    }
    if (ret == HAL_TIMEOUT) {
        Logger::log(LogLevel::WARNING, "UART Receive operation timed out.");
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;
    }

    Logger::log(LogLevel::INFO, "Data received successfully, size: %d bytes.", *size);
    return MemoryErrorHandling::MEM_NO_ERROR;
}
#else
MEM_ERROR UARTWrapper::ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
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
            uartProperties->m_UARTHandle.Instance = m_AvailableUARTPorts[elemCtr].GetUARTHandle();
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


    uartProperties->m_UARTHandle.Init.BaudRate = uartProperties->m_Baudrate;

    if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_5 ||
        uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_6 ||
        uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_7)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;
    else if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_8)
        uartProperties->m_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
    else if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_9)
        uartProperties->m_UARTHandle.Init.WordLength = UART_WORDLENGTH_9B;

    if (uartProperties->m_StopBits == UARTWrapper_STOP_BITS_1)
        uartProperties->m_UARTHandle.Init.StopBits = UART_STOPBITS_1;
    else if(uartProperties->m_StopBits == UARTWrapper_STOP_BITS_2)
        uartProperties->m_UARTHandle.Init.StopBits = UART_STOPBITS_2;

    if(uartProperties->m_Parity == UARTWrapper_NO_PARITY)
        uartProperties->m_UARTHandle.Init.Parity = UART_PARITY_NONE;
    else if(uartProperties->m_Parity == UARTWrapper_PARITY_EVEN)
        uartProperties->m_UARTHandle.Init.Parity = UART_PARITY_EVEN;
    else if(uartProperties->m_Parity == UARTWrapper_PARITY_ODD)
         uartProperties->m_UARTHandle.Init.Parity = UART_PARITY_ODD;

    if (uartProperties->m_Mode == UARTWrapper_TRANSMIT)
        uartProperties->m_UARTHandle.Init.Mode = UART_MODE_TX;
    if (uartProperties->m_Mode == UARTWrapper_RECEIVE)
        uartProperties->m_UARTHandle.Init.Mode = UART_MODE_RX;
    if (uartProperties->m_Mode == UARTWrapper_TRANSMIT_RECEIVE)
        uartProperties->m_UARTHandle.Init.Mode = UART_MODE_TX_RX;

    uartProperties->m_UARTHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartProperties->m_UARTHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    uartProperties->m_UARTHandle.gState = HAL_UART_STATE_RESET;
    if (HAL_UART_Init(&uartProperties->m_UARTHandle) != HAL_OK) {
        Logger::log(LogLevel::ERROR, "HAL_UART_Init failed!");
        return MemoryErrorHandling::MEM_HAL_INTERNAL_ERROR;
    }
    Logger::log(LogLevel::DEBUG, "Interface %s initialized", m_UARTHandle->m_InterfaceName);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

void UARTWrapper::SendData(uint8_t* msg, uint16_t msg_len, uint32_t timeout) {
    Logger::log(LogLevel::INFO, "Send data: %s", msg);

    // Transmit the data over UART
    auto returnValue = HAL_UART_Transmit(&m_UARTHandle->m_UARTHandle, msg, msg_len, timeout);
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
  uint8_t data[512] ;// TODO size
  uint16_t real_size;
    Logger::log(LogLevel::DEBUG, "Waiting for data to receive");
    switch (HAL_UARTEx_ReceiveToIdle(&m_UARTHandle->m_UARTHandle, data, size, &real_size, timeout)) {
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
MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTHandle *uartHandle)
{
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}
#endif // STM32