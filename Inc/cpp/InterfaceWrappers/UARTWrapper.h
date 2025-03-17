/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
#include "InterfaceWrapper.h"
#include "UARTProperties.h"

#include <vector>


using namespace UARTProperties;

/**
 * @brief This class wrapps the device specific UART implementation.
 * It implements the functions to initialize, read and write from and to the interface.
 */
class UARTWrapper : public InterfaceWrapper
{
public:
    explicit UARTWrapper(const char* interfaceName,
                       uint32_t baudrate = 9600,
                       UARTProperties::Mode mode = UARTWrapper_TRANSMIT_RECEIVE,
                       WordLength wordLen = UARTWrapper_WORD_LENGTH_8,
                       Parity parity = UARTWrapper_NO_PARITY,
                       UART_StopBits stopBits = UARTWrapper_STOP_BITS_1);

    ~UARTWrapper() override;

    MEM_ERROR Initialize() override;

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    void SendData(uint8_t* msg, uint16_t msg_len, uint32_t timeout);
    vector<uint8_t> ReceiveToIdle(uint16_t size, uint32_t timeout);

private:
    MEM_ERROR InitializeUARTDeviceSpecific(UARTHandle *uartProperties);

    UARTHandle *m_UARTHandle;
    std::vector<AvailableUARTProperties> m_AvailableUARTPorts;
};
#endif //STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
