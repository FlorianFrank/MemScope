//
// Created by florianfrank on 07.12.21.
//

#ifndef STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H

#include "cpp/MemoryErrorHandling.h"
#include <cstdint>



#define STM32 1
#if STM32
extern "C" {
#include "SystemFiles/spi.h"
#include <stm32f4xx_hal_spi.h>
};
#endif // TODO

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class SPIWrapper
{

public:


    SPIWrapper();

    static void SetWriteProtect();
    static void ResetWriteProtect();

    static void SetChipSelect();
    static void ResetChipSelect();

private:
    SPI_HandleTypeDef hspi5;


};


#endif //STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
