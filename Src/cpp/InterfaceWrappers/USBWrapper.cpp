/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/InterfaceWrappers/USBWrapper.h"

char MY_RX_BUFFER[CDC_DATA_HS_MAX_PACKET_SIZE]= {0};


void USBWrapper::receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize)
{

}


void USBWrapper::showHelpUSB()
{

}

void USBWrapper::executeCommandUSB()
{

}

void USBWrapper::USBCDCRXCallback(uint8_t *Buf, uint32_t len)
//uint8_t VCP_read(uint8_t* Buf, uint32_t len)
{
}

MEM_ERROR USBWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    // TODO
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR USBWrapper::ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    // TODO
    return MemoryErrorHandling::MEM_NO_ERROR;
}
