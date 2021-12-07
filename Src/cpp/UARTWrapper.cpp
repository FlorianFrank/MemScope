//
// Created by florianfrank on 07.12.21.
//

#include "cpp/UARTWrapper.h"


/*
 * @brief								rewritten function to receive with a delay of 10ms
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 * @param uint8_t *dstBuffer			the destination buffer
 * @param uint32_t bufferSize			the buffer size
 */
void UARTWrapper::receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize){
    HAL_UART_Receive_IT(huart, dstBuffer, bufferSize);
    HAL_Delay(10);
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