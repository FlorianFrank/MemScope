/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H

#include "InterfaceWrapper.h"
#include "UARTProperties.h"

using namespace UARTProperties;

class UARTWrapper : public InterfaceWrapper
{
public:
    explicit UARTWrapper(const char* interfaceName, uint32_t baudrate = 9600, UARTProperties::Mode mode = UARTWrapper_TRANSMIT_RECEIVE, WordLength wordLen = UARTWrapper_WORD_LENGTH_8,
                         Parity parity = UARTWrapper_NO_PARITY, UART_StopBits stopBits = UARTWrapper_STOP_BITS_1);

    virtual ~UARTWrapper();

    MEM_ERROR Initialize() override;

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

private:
    UARTHandle *m_UARTHandle;
    static AvailableUARTProperties availableUARTPorts[];

    // Device specific functions
    static MEM_ERROR InitializeUARTDeviceSpecific(UARTHandle *uartProperties);

};


#endif //STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
