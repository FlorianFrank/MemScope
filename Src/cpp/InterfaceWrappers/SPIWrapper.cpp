/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include <cstring>
#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "io_pin_defines.h"

#if STM32
extern "C" {
#include "SystemFiles/spi.h"
#include <stm32f4xx_hal_spi.h>
};
#endif // STM32

/*static*/ AvailableSPIProperties SPIWrapper::availableSPIPorts[]
{
        {SPI1, "SPI1", "PF0", "PF0", "PF0", "PF0"},
        {SPI2, "SPI1", "PF0", "PF0", "PF0", "PF0"},
        {SPI3, "SPI1", "PF0", "PF0", "PF0", "PF0"},
        {SPI4, "SPI1", "PF0", "PF0", "PF0", "PF0"},
        {SPI5, "SPI1", "PF0", "PF0", "PF0", "PF0"},
        {SPI6, "SPI1", "PF0", "PF0", "PF0", "PF0"},
};

SPIWrapper::SPIWrapper(const char *interfaceName, SPI_Baudrate_Prescaler prescaler, SPI_Clock_Phase clockPhase,
                       SPI_Clock_Polarity clockPolarity, SPI_Mode spiMode)
{
    m_SPIHandle = new SPIProperties();
    m_SPIHandle->m_InterfaceName = interfaceName;
    m_SPIHandle->m_Prescaler = prescaler;
    m_SPIHandle->m_ClockPhase = clockPhase;
    m_SPIHandle->m_ClockPolarity = clockPolarity;
    m_SPIHandle->m_Mode = spiMode;
}

SPIWrapper::~SPIWrapper()
{
    delete m_SPIHandle;
}


MEM_ERROR SPIWrapper::Initialize()
{
    return InitializeSPIInterface(m_SPIHandle);
}

#if STM32
MEM_ERROR SPIWrapper::InitializeSPIInterface(SPIProperties *spiProperties)
{
    int elemCtr = 0;
    bool interfaceFound = false;
    for(AvailableSPIProperties availPorts: availableSPIPorts)
    {
        if(strcmp(availPorts.m_name, spiProperties->m_InterfaceName) == 0)
        {
            spiProperties->m_SPIHandle.Instance = availableSPIPorts[elemCtr].m_UARTHandle;
            interfaceFound = true;
            break;
        }
        elemCtr++;
    }

    if(!interfaceFound)
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;

    spiProperties->m_SPIHandle.Init.BaudRatePrescaler = spiProperties->m_Prescaler; // TODO

    if(m_SPIHandle->m_Mode == SPI_MASTER)
        hspi4.Init.Mode = SPI_MODE_MASTER;
    else if(m_SPIHandle->m_Mode == SPI_SLAVE)
        hspi4.Init.Mode = SPI_MODE_SLAVE;

    if(m_SPIHandle->m_ClockPolarity == SPIWrapper_CPOL_LOW)
        spiProperties->m_SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    else if(m_SPIHandle->m_ClockPolarity == SPIWrapper_CPOL_HIGH)
        spiProperties->m_SPIHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;

    if(m_SPIHandle->m_ClockPhase == SPIWrapper_CP_1_EDGE)
        hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
    else if(m_SPIHandle->m_ClockPhase == SPIWrapper_CP_2_EDGE)
        hspi4.Init.CLKPhase = SPI_PHASE_2EDGE;
    /* USER CODE END SPI4_Init 1 */

    // TODO set remaining parameters
    hspi4.Init.Direction = SPI_DIRECTION_2LINES;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.NSS = SPI_NSS_SOFT;
    hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi4.Init.CRCPolynomial = 10;
    return MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Init(&hspi4));
}
#else
MEM_ERROR SPIWrapper::InitializeSPIInterface(SPIProperties *spiProperties)
{
    return MemoryErrorHandling::MEM_NO_ERROR;
}
#endif
/**
 * @brief Sets the Write Protect Pin on pin PC6
 */
/*static*/ void SPIWrapper::SetWriteProtect()
{
#if STM32
    HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_RESET);
#endif // STM32
}

/**
 * @brief Resets the Write Protect Pin. On the STM32 on pin PC6.
 */
/*static*/ void SPIWrapper::ResetWriteProtect()
{
#if STM32
    HAL_GPIO_WritePin(SPI5_WP_GPIO_Port, SPI5_WP_Pin, GPIO_PIN_SET);
#endif // STM32
}

/**
 * @brief Sets the Chip Select Pin. On STM32 on pin PF6
 */
/*static*/ void SPIWrapper::SetChipSelect()
{
#if STM32
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
#endif // STM32
}

/**
 * @brief Resets the Chip Select Pin. On STM32 on pin PF6
 */
/*static*/ void SPIWrapper::ResetChipSelect()
{
#if STM32
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
#endif // STM32
}

/**
 * @brief Send data on the specified SPI interface. CS must be set manually!
 * @param data byte buffer to send.
 * @param size input size to send.
 * @param timeout timeout when using blocking SPI functions.
 * @return MEM_NO_ERROR if execution was successful.
 */
MEM_ERROR SPIWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    if(!size || *size == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

#if STM32
    return MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Transmit(&m_SPIHandle->m_SPIHandle, data, *size, timeout));
#else
    m_SPIHandle.StoreBuffer(data, size);
    return MemoryErrorHandling::MEM_NO_ERROR;
#endif // STM32
}

/**
 * @brief Receives data on the specified SPI interface. CS must be set manually!
 * @param data byte buffer which is returned.
 * @param size the input specifies the maximum size of the read buffer. The same buffer is used to set the output size.
 * @param timeout timeout in ms when using blocking functions.
 * @return MEM_NO_ERROR if execution was successful.
 */
MEM_ERROR SPIWrapper::ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout)
{
    if(!size || *size == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

#if STM32
    return MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Receive(&m_SPIHandle->m_SPIHandle, data, *size, timeout));
#else
    m_SPIHandle.ReadBuffer(data, size);
    return MemoryErrorHandling::MEM_NO_ERROR;
#endif // STM32
}

