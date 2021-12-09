/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "io_pin_defines.h"

#if STM32
extern "C" {
#include "SystemFiles/spi.h"
#include <stm32f4xx_hal_spi.h>
};
#endif // STM32

SPIWrapper::SPIWrapper(SPIHandle spiHandle): m_SPIHandle(spiHandle) {}

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
    return MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Transmit(m_SPIHandle, data, *size, timeout));
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
    return MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Receive(m_SPIHandle, data, *size, timeout));
#else
    m_SPIHandle.ReadBuffer(data, size);
    return MemoryErrorHandling::MEM_NO_ERROR;
#endif // STM32
}