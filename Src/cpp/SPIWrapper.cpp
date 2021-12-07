//
// Created by florianfrank on 07.12.21.
//

#include "cpp/SPIWrapper.h"

MEM_ERROR SPIWrapper::SetWriteProtect()
{
#if STM32 // TODO
    HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_SET);
#endif // STM32
}

MEM_ERROR SPIWrapper::ResetWriteProtect()
{
#if STM32 // TODO
    HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_RESET);
#endif // STM32
}

SPIWrapper::SPIWrapper()
{
#if STM32 // TODO
#endif // STM32
}
