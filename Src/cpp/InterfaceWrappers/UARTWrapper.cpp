/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/InterfaceWrappers/UARTWrapper.h"


/**
 * @brief This array contains the list of all possible interfaces on the STM32F429-Disc1 chip.
 * It contains the interface, the minimum and maximum baudrate as well as the RX and TX Pins.
 *
 */
/*static*/ AvailableUARTProperties UARTWrapper::availableUARTPorts[] = {
#if STM32F429xx
        {USART1, "USART1", 9600, 2000000, {IO_BANK_A, IO_PIN_10}, {IO_BANK_A, IO_PIN_9}},
        {USART2, "USART2", 9600, 2000000, {IO_BANK_D, IO_PIN_5}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
        {USART3, "USART3", 9600, 2000000, {IO_BANK_D, IO_PIN_5}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
        {UART4, "UART4", 9600, 2000000, {IO_BANK_C, IO_PIN_11}, {IO_BANK_C, IO_PIN_10}},
        {UART5, "UART5", 9600, 2000000, {IO_BANK_D, IO_PIN_5}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
        {USART6, "USART6", 9600, 2000000,{IO_BANK_D, IO_PIN_5}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
        {UART7, "UART7", 9600, 2000000, {IO_BANK_D, IO_PIN_5}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
        {UART8, "UART8", 9600, 2000000, {IO_BANK_D, IO_PIN_5}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
#endif // STM32F429xx
};


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
}


UARTWrapper::~UARTWrapper()
{
    delete m_UARTHandle;
}

/*
 * @brief								callback function called if something is received via UART
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 */
void UARTWrapper::HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    // clear Rx_Buffer before receiving new data
}

MEM_ERROR UARTWrapper::Initialize()
{
    return InitializeUARTDeviceSpecific(m_UARTHandle);
}


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

MEM_ERROR UARTWrapper::ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    HAL_StatusTypeDef ret = HAL_UART_Receive(&m_UARTHandle->m_UARTHandle, data, *size, timeout);
    if(ret == HAL_BUSY)
        return MemoryErrorHandling::MEM_HAL_BUSY;
    if (ret == HAL_ERROR)
        return MemoryErrorHandling::MEM_IO_ERROR;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

#if STM32
/*static*/ MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTHandle *uartProperties)
{
    int elemCtr = 0;
    bool interfaceFound = false;
    for(const AvailableUARTProperties& availPorts: availableUARTPorts)
    {
        if(availPorts.m_name == uartProperties->m_InterfaceName)
        {
            uartProperties->m_UARTHandle.Instance = availableUARTPorts[elemCtr].m_UARTHandle;
            interfaceFound = true;
            break;
        }

        elemCtr++;
    }

    if(!interfaceFound)
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;

    uartProperties->m_UARTHandle.Init.BaudRate = uartProperties->m_Baudrate;

    if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_5 ||
        uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_6 ||
        uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_7)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;
    else if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_8)
        uartProperties->m_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
    else if (uartProperties->m_WordLength == UARTWrapper_WORD_LENGTH_9)
        uartProperties->m_UARTHandle.Init.WordLength = UART_WORDLENGTH_9B;

    if(uartProperties->m_StopBits == UARTWrapper_STOP_BITS_1)
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
    if (HAL_UART_Init(&uartProperties->m_UARTHandle) != HAL_OK)
        return MemoryErrorHandling::MEM_HAL_INTERNAL_ERROR;

    return MemoryErrorHandling::MEM_NO_ERROR;
}


#else
/*static*/ MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTHandle *uartHandle)
{
    return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
}
#endif // STM32




