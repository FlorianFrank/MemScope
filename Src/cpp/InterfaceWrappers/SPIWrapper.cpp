//
// Created by florianfrank on 07.12.21.
//

#include <cstdio>
#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "io_pin_defines.h"


/*static*/ void SPIWrapper::SetWriteProtect()
{
#if STM32 // TODO
    HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_SET);
#endif // STM32
}

/*static*/ void SPIWrapper::ResetWriteProtect()
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






/*static*/ void SPIWrapper::SetChipSelect()
{
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);

}

/*static*/ void SPIWrapper::ResetChipSelect()
{
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
}

