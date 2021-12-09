/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H

#include "cpp/MemoryErrorHandling.h"
#include "cpp/InterfaceWrappers.h"
#include <cstdint>

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;


#if STM32
struct __SPI_HandleTypeDef;
typedef struct __SPI_HandleTypeDef* SPIHandle;
#elif UNIT_TEST
#include "TestInterfaceWrapper.h"
typedef TestInterfaceWrapper& SPIHandle;
#endif // STM32

class SPIWrapper : public InterfaceWrappers
{

public:
    explicit SPIWrapper(SPIHandle spiHandle);

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) override;
    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    static void SetWriteProtect();
    static void ResetWriteProtect();

    static void SetChipSelect();
    static void ResetChipSelect();

private:
    SPIHandle m_SPIHandle;
};


#endif //STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
