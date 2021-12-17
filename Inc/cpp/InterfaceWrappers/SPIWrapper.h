/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
#define STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H


#include "InterfaceWrappers.h"
#include "SPIProperties.h"
#include "cpp/MemoryErrorHandling.h"

#include <cstdint>

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

/**
 * @brief This class serves as wrapper for SPI communication.
 * Currently this wrapper only supports an STM32F429DISC1 board.
 */
class SPIWrapper : public InterfaceWrappers
{
public:
    explicit SPIWrapper(const char *interfaceName, SPI_Mode spiMode = SPI_MASTER,
                        SPI_Baudrate_Prescaler prescaler = SPI_Prescaler_2,
                        SPI_Clock_Phase clockPhase = SPIWrapper_CP_1_EDGE,
                        SPI_Clock_Polarity clockPolarity = SPIWrapper_CPOL_LOW);

    MEM_ERROR Initialize() override;

    MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) override;
    MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) override;

    static void SetWriteProtect();
    static void ResetWriteProtect();

    static void SetChipSelect();
    static void ResetChipSelect();

    virtual ~SPIWrapper();

private:
    SPIProperties* m_SPIHandle;
    static AvailableSPIProperties availableSPIPorts[];
    MEM_ERROR InitializeSPIInterface(SPIProperties *spiProperties);
};


#endif //STM_MEASUREMENT_FIRMWARE_SPIWRAPPER_H
