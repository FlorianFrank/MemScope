/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H

#include "cpp/InterfaceWrappers.h"

extern "C" {
#include <stm32f4xx_hal_uart.h>
};

#if STM32
struct __UART_HandleTypeDef;
typedef struct __UART_HandleTypeDef __UART_HandleTypeDef;
#endif // STM32

class UARTWrapper : public InterfaceWrappers
{
public:
    UARTWrapper();

    void receive(UART_HandleTypeDef *huart, uint8_t *dstBuffer, uint32_t bufferSize);

    MEM_ERROR SendData(uint8_t *data, const uint16_t *size, uint32_t timeout) override;

    MEM_ERROR ReceiveData(uint8_t *data, const uint16_t *size, uint32_t timeout) override;

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
};


#endif //STM_MEASUREMENT_FIRMWARE_UARTWRAPPER_H
