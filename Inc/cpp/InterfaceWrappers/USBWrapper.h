//
// Created by florianfrank on 07.12.21.
//

#ifndef STM_MEASUREMENT_FIRMWARE_USBWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_USBWRAPPER_H

#include "cpp/InterfaceWrappers.h"

// TODO
#define STM32 1
#if STM32
extern "C" {
#include <usbd_cdc.h>
};
#endif // STM32

class USBWrapper : public InterfaceWrappers
{
public:
    void receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize);

    void showHelpUSB();

    void executeCommandUSB();

    void USBCDCRXCallback(uint8_t *Buf, uint32_t len);

    MEM_ERROR SendData(uint8_t *data, const uint16_t *size, uint32_t timeout) override;

    MEM_ERROR ReceiveData(uint8_t *data, const uint16_t *size, uint32_t timeout) override;

};


#endif //STM_MEASUREMENT_FIRMWARE_USBWRAPPER_H
