//
// Created by florianfrank on 07.12.21.
//

#ifndef STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H

#include "MemoryErrorHandling.h"

#if STM32
#include "SystemFiles/spi.h"
#endif // TODO

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class SPIWrapper
{

public:
    SPIWrapper();
    MEM_ERROR SetWriteProtect();
    MEM_ERROR ResetWriteProtect();

};


#endif //STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
