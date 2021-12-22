/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_SPIPROPERTIES_H
#define MEMORY_TESTING_FW_SPIPROPERTIES_H

#include "cpp/Devices/DeviceDefines.h"

#if STM32
#include <stm32f4xx_hal_spi.h>
#endif // STM32

#include <string>


#if STM32
struct __SPI_HandleTypeDef;
typedef struct __SPI_HandleTypeDef SPIIdentifier;
#elif UNIT_TEST
#include "TestInterfaceWrapper.h"
    typedef TestInterfaceWrapper& SPIIdentifier;
#endif // STM32

namespace SPIProperties
{

    enum BaudratePrescaler
    {
        SPI_Prescaler_2 = 2,
        SPI_Prescaler_4 = 4,
        SPI_Prescaler_8 = 8,
        SPI_Prescaler_16 = 16,
        SPI_Prescaler_32 = 32,
        SPI_Prescaler_64 = 64,
        SPI_Prescaler_128 = 128,
        SPI_Prescaler_256 = 256,
    };

/**
 * @brief This enum contains the parameters for the clock polarity.
 * Data is send either at a transition from high to low or vice versa.
 */
    enum ClockPoloarity
    {
        SPIWrapper_CPOL_LOW, SPIWrapper_CPOL_HIGH
    };

/**
 * @brief This enum contains the parameters setting the clock phase.
 * when 1 edge is set, data is sampled on rising and shifted out on falling edge.
 * when 2 edge is set the interface behaves in opposite direction.
 */
    enum ClockPhase
    {
        SPIWrapper_CP_1_EDGE, SPIWrapper_CP_2_EDGE,
    };

/**
 * @brief Enum to select if the interface should be opened in transmit, receive or bidirectional mode.
 */
    enum Mode
    {
        SPI_MASTER, SPI_SLAVE,
    };


/**
 * @brief This struct contains all properties of an UART interface.
 * It is internally stored in the UARTWrapper class.
 */
    struct SPIHandle
    {
        SPIIdentifier m_SPIHandle;
        std::string m_InterfaceName;
        BaudratePrescaler m_Prescaler;
        Mode m_Mode;
        ClockPoloarity m_ClockPolarity;
        ClockPhase m_ClockPhase;
    };

/**
 * @brief This struct stores value of all
 */
    struct AvailableSPIProperties
    {
#if STM32
        SPI_TypeDef *m_UARTHandle;
#endif
        const char m_name[10];
        GPIOPin m_MISOPin;
        GPIOPin m_MOSIPin;
        GPIOPin m_CLKPin;
        GPIOPin m_CSPin;
    };
}

#endif //MEMORY_TESTING_FW_SPIPROPERTIES_H
