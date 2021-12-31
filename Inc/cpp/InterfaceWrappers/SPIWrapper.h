/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
#include "InterfaceWrapper.h"
#include "SPIProperties.h"
#include "cpp/MemoryErrorHandling.h"
#include "cpp/Devices/DeviceWrapper.h"

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;
using namespace SPIProperties;

/**
 * @brief This class serves as wrapper for SPI communication.
 * Currently this wrapper only supports an STM32F429DISC1 board.
 */
class SPIWrapper : public InterfaceWrapper
{
public:
    explicit SPIWrapper(const char *interfaceName, Mode spiMode = SPI_MASTER,
                        BaudratePrescaler prescaler = SPI_Prescaler_2,
                        ClockPhase clockPhase = SPIWrapper_CP_1_EDGE,
                        ClockPoloarity clockPolarity = SPIWrapper_CPOL_LOW);

    ~SPIWrapper() override;

    MEM_ERROR Initialize() override;

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeoutInMs) override;
    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeoutInMs) override;

    static void SetWriteProtect();
    static void ResetWriteProtect();

    static void SetChipSelect();
    static void ResetChipSelect();

    inline static bool ReadChipSelect();

private:
    SPIHandle* m_SPIHandle;
    MEM_ERROR InitializeSPIInterface(SPIHandle *spiProperties);

    DeviceWrapper* m_DeviceWrapper;
};


#endif //STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
