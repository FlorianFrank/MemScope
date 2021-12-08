//
// Created by florianfrank on 07.12.21.
//

#include "cpp/InterfaceWrappers/USBWrapper.h"

char MY_RX_BUFFER[CDC_DATA_HS_MAX_PACKET_SIZE]= {0};


void USBWrapper::receiveUSB(uint8_t *dstBuffer, uint32_t bufferSize)
{

};


void USBWrapper::showHelpUSB()
{

};

void USBWrapper::executeCommandUSB()
{

};

void USBWrapper::USBCDCRXCallback(uint8_t *Buf, uint32_t len)
//uint8_t VCP_read(uint8_t* Buf, uint32_t len)
{
    if (!Buf)
        return;
//uint32_t count=0;
//uint8_t myBuf = *Buf;
//char myChar = (char) myBuf;
    memcpy(MY_RX_BUFFER, Buf, len);
    uint32_t MY_RX_BUFFER_LEN = len;

/* Check inputs */
    if (!Buf || (len == 0))
    {
        return;// 0;
    } else
    {
        for (uint32_t i = 0; i < MY_RX_BUFFER_LEN; i++)
        {
            Rx_Data[0] = MY_RX_BUFFER[i];

//Rx_Data[0] = (char)(*Buf++);

            if (Rx_Index == 0)
            {
                clearBuffer(0);
            }

// if received data different from ascii 13 (Enter)
            if (Rx_Data[0] != 13)
            {
// only allow 0-9, A-Z, a-z, SP (32), DEL (127)
                if (Rx_Data[0] == 32 || Rx_Data[0] == 127 || (Rx_Data[0] > 47 && Rx_Data[0] < 58) ||
                    (Rx_Data[0] > 64 && Rx_Data[0] < 91) || (Rx_Data[0] > 96 && Rx_Data[0] < 123))
                {
                    if (Rx_Data[0] == 127)
                    {
                        if (Rx_Index > 0)
                        {
                            Rx_Index--;
                        }
// correct extended ascii chars which uses two bytes when press 'Delete'
                        if (Rx_Buffer[Rx_Index] > 127 && Rx_Index > 0)
                        {
                            Rx_Index--;
                        }
                    } else
                    {
                        Rx_Buffer[Rx_Index++] = Rx_Data[0];
                    }
// print input char by char

                    sprintf(STRING_BUFFER, "%c", Rx_Data[0]);
                    len = strlen(STRING_BUFFER);
//                    sendUSB((uint8_t *) m_SendBuffer, len);
                }
            } else
            {
//				sprintf(m_SendBuffer, "Detected carriage Return\r\n");
//				len = strlen(m_SendBuffer);
//				sendUSB((uint8_t *)m_SendBuffer, len);

// if received data = 13
                Rx_Index = 0;
                Transfer_cplt = 1; // transfer complete, data is ready to read
            }

        }

    }
}

MEM_ERROR USBWrapper::SendData(uint8_t *data, const uint16_t *size, uint32_t timeout)
{
    // TODO
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR USBWrapper::ReceiveData(uint8_t *data, const uint16_t *size, uint32_t timeout)
{
    // TODO
    return MemoryErrorHandling::MEM_NO_ERROR;
}
