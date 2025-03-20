/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_USBWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_USBWRAPPER_H

#include "InterfaceWrapper.h"

#if STM32
extern "C" {
#include <usbd_cdc.h>
}
#endif // STM32

class USBWrapper : public InterfaceWrapper
{
public:
    void receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize);

    void showHelpUSB();

    void executeCommandUSB();

    void USBCDCRXCallback(uint8_t *Buf, uint32_t len);

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout, bool forceFlush=true) override;

    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, BlockingMode blockingMode, uint32_t timeout) override;

};
#endif //STM_MEASUREMENT_FIRMWARE_USBWRAPPER_H
