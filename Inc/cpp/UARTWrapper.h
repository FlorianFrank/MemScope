//
// Created by florianfrank on 07.12.21.
//

#ifndef STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H


class UARTWrapper
{

    void receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize);
/*
 * @brief								callback function called if something is received via UART
 * @param UART_HandleTypeDef *huart		the UART handler to communicate with the user
 */
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
};


#endif //STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
