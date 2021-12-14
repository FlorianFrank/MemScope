/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H

#include "InterfaceWrappers.h"
#include "UARTProperties.h"

class UARTWrapper : public InterfaceWrappers
{
public:
    explicit UARTWrapper(const char* interfaceName, uint32_t baudrate = 9600, UART_Mode mode = UARTWrapper_TRANSMIT_RECEIVE, UART_WordLength wordLen = UARTWrapper_WORD_LENGTH_8,
                         UART_Partiy parity = UARTWrapper_NO_PARITY, UART_StopBits stopBits = UARTWrapper_STOP_BITS_1);

    MEM_ERROR Initialize() override;

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

private:
    UARTProperties *m_UARTHandle;
    static AvailableUARTProperties availableUARTPorts[];

    // Device specific functions
    static MEM_ERROR InitializeUARTDeviceSpecific(UARTProperties *uartProperties);

};


#endif //STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
