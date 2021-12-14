/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include <cstring>
#include "cpp/InterfaceWrappers/UARTWrapper.h"


/*static*/ AvailableUARTProperties UARTWrapper::availableUARTPorts[] = {
#if STM32F429xx
        {UART4, "UART4", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {UART5, "UART5", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {UART7, "UART7", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {UART8, "UART8", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {USART1, "USART1", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {USART2, "USART2", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {USART3, "USART3", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
        {USART6, "USART6", 9600, 2000000, "PF0", "PF1"}, // TODO Pins anpassen
#endif // STM32F429xx
};


UARTWrapper::UARTWrapper(const char* interfaceName, uint32_t baudrate, UART_Mode mode, UART_WordLength wordLen,
                                                  UART_Partiy parity, UART_StopBits stopBits)
{
    m_UARTHandle = new UARTProperties;
    m_UARTHandle->m_InterfaceName = interfaceName;
    m_UARTHandle->m_Baudrate = baudrate;
    m_UARTHandle->m_Mode = mode;
    m_UARTHandle->m_WordLength = wordLen;
    m_UARTHandle->m_Parity = parity;
    m_UARTHandle->m_StopBits = stopBits;
}

/*
 * @brief								callback function called if something is received via UART
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 */
void UARTWrapper::HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    // clear Rx_Buffer before receiving new data
    if(huart->Instance == UART4){
        if(Rx_Index == 0){
            clearBuffer(0);
        }
        // if received data different from ascii 13 (Enter)
        if(Rx_Data[0] != 13){
            // only allow 0-9, A-Z, a-z, SP (32), DEL (127)
            if(Rx_Data[0] == 32 || Rx_Data[0] == 127 || (Rx_Data[0] > 47 && Rx_Data[0] < 58) || (Rx_Data[0] > 64 && Rx_Data[0] < 91) ||
               (Rx_Data[0] > 96 && Rx_Data[0] < 123)){
                if(Rx_Data[0] == 127){
                    if(Rx_Index > 0){
                        Rx_Index--;
                    }
                    // correct extended ascii chars which uses two bytes when press 'Delete'
                    if(Rx_Buffer[Rx_Index] > 127 && Rx_Index > 0){
                        Rx_Index--;
                    }
                    //clearBuffer(Rx_Index);
                }else{
                    Rx_Buffer[Rx_Index++] = Rx_Data[0];
                }
                // print input char by char
                HAL_UART_Transmit_IT(huart, (uint8_t *)Rx_Data, 1);
            }
        }else{
            // if received data = 13
            Rx_Index = 0;
            Transfer_cplt = 1; // transfer complete, data is ready to read
        }
        HAL_UART_Receive_IT(huart, (uint8_t *)Rx_Data, 1);
    }
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
/*static*/ MEM_ERROR UARTWrapper::InitializeUARTDeviceSpecific(UARTProperties *uartProperties)
{

    int elemCtr = 0;
    for(AvailableUARTProperties availPorts: availableUARTPorts)
    {
        if(strcmp(availPorts.m_name, uartProperties->m_InterfaceName) == 0)
        {
            uartProperties->m_UARTHandle.Instance = availableUARTPorts[elemCtr].m_UARTHandle;
            break;
        }

        elemCtr++;
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

    if(uartProperties->m_StopBits == UARTWrapper_STOP_BITS_1)
        uartProperties->m_UARTHandle.Init.StopBits = UART_STOPBITS_1;
    else
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




