/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/Devices/DeviceWrapper.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "cpp/TimeMeasurement.h"

#include "io_pin_defines.h"

#if STM32
#if STM32F429xx
#include "cpp/Devices/STM32F429Wrapper.h"
extern "C" {
#include "SystemFiles/spi.h"
#include <stm32f4xx_hal_spi.h>
}
#endif // STM32F429xx
#endif // STM32


SPIWrapper::SPIWrapper(const char *interfaceName, Mode spiMode, BaudratePrescaler prescaler, ClockPhase clockPhase,
                       ClockPoloarity clockPolarity)
{
    m_SPIHandle = static_cast<SPIHandle *>(malloc(sizeof(SPIHandle)));
    m_SPIHandle->m_Prescaler = prescaler;
    m_SPIHandle->m_ClockPhase = clockPhase;
    m_SPIHandle->m_ClockPolarity = clockPolarity;
    m_SPIHandle->m_Mode = spiMode;


#if STM32
    m_DeviceWrapper = new STM32F429Wrapper();
    m_AvailableSPIPorts = {
            {SPI1, "SPI1", {IO_BANK_B,         IO_PIN_4},         {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_A,         IO_PIN_5},         {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
            {SPI2, "SPI2", {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
            {SPI3, "SPI3", {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
            {SPI4, "SPI4", {IO_BANK_E,         IO_PIN_5},         {IO_BANK_E,         IO_PIN_6},         {IO_BANK_E,         IO_PIN_2},         {IO_BANK_E,         IO_PIN_4}},
            {SPI5, "SPI5", {IO_BANK_F,         IO_PIN_8},         {IO_BANK_F,         IO_PIN_9},         {IO_BANK_F,         IO_PIN_7},         {IO_BANK_F,         IO_PIN_6}},
            {SPI6, "SPI6", {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED}},
            };
#else
    m_DeviceWrapper = new DeviceWrapper();
#endif
}

SPIWrapper::~SPIWrapper()
{
    delete m_SPIHandle;
    delete m_DeviceWrapper;
}


MEM_ERROR SPIWrapper::Initialize()
{
    return InitializeSPIInterface(m_SPIHandle);
}

#if STM32
/**
 * @brief This function initializes the handle to read and write the SPI interface using a STM32 board.
 * @param spiProperties properties like the clock polarity, clock phase or prescaler to set.
 * @return MEM_NO_ERROR if no error occurs.
 */
MEM_ERROR SPIWrapper::InitializeSPIInterface(SPIHandle *spiProperties)
{
    m_DeviceWrapper->InitializeHardwareInterface(spiProperties->m_InterfaceName);
    int elemCtr = 0;
    bool interfaceFound = false;
    for(const AvailableSPIProperties& availPorts: m_AvailableSPIPorts)
    {
        if(availPorts.GetName() == spiProperties->m_InterfaceName)
        {
            spiProperties->m_SPIHandle.Instance = m_AvailableSPIPorts[elemCtr].GetSPIHandle();
            interfaceFound = true;
            break;
        }
        elemCtr++;
    }

    if(!interfaceFound)
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;

    spiProperties->m_SPIHandle.Init.BaudRatePrescaler = spiProperties->m_Prescaler; // TODO

    if(m_SPIHandle->m_Mode == SPI_MASTER)
        spiProperties->m_SPIHandle.Init.Mode = SPI_MODE_MASTER;
    else if(m_SPIHandle->m_Mode == SPI_SLAVE)
        spiProperties->m_SPIHandle.Init.Mode = SPI_MODE_SLAVE;

    if(m_SPIHandle->m_ClockPolarity == SPIWrapper_CPOL_LOW)
        spiProperties->m_SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    else if(m_SPIHandle->m_ClockPolarity == SPIWrapper_CPOL_HIGH)
        spiProperties->m_SPIHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;

    if(m_SPIHandle->m_ClockPhase == SPIWrapper_CP_1_EDGE)
        spiProperties->m_SPIHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    else if(m_SPIHandle->m_ClockPhase == SPIWrapper_CP_2_EDGE)
        spiProperties->m_SPIHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
    /* USER CODE END SPI4_Init 1 */

    // TODO set remaining parameters
    spiProperties->m_SPIHandle.Init.Direction = SPI_DIRECTION_2LINES;
    spiProperties->m_SPIHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    spiProperties->m_SPIHandle.Init.NSS = SPI_NSS_SOFT;
    spiProperties->m_SPIHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spiProperties->m_SPIHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    spiProperties->m_SPIHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spiProperties->m_SPIHandle.Init.CRCPolynomial = 10;
    ResetChipSelect();
    return MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Init(&spiProperties->m_SPIHandle));
}
#else
MEM_ERROR SPIWrapper::InitializeSPIInterface(SPIHandle *spiProperties)
{
    m_SPIHandle = spiProperties;
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
/*static*/ inline void SPIWrapper::ResetChipSelect()
{
#if STM32
    HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
#endif // STM32
}

/**
 * @brief Send data on the specified SPI interface. CS must be set manually!
 * @param data byte buffer to send.
 * @param size input size to send.
 * @param timeoutInMs timeoutInMs when using blocking SPI functions.
 * @return MEM_NO_ERROR if execution was successful.
 */
MEM_ERROR SPIWrapper::SendData(uint8_t *data, uint16_t *size, uint32_t timeoutInMs, bool forceFlush)
{
    if(!size || *size == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    bool retCS = ReadChipSelect();

    TimeMeasurement tm{};
    tm.ResetAndStartTimer();
    while(!retCS)
    {
        if(tm.GetElapsedTimeInMS())
            return MemoryErrorHandling::MEM_HAL_TIMEOUT;
        retCS = ReadChipSelect();
    }

    bool writeEnable = false;
    MEM_ERROR ret;
    if(m_SPIHandle->m_Mode == SPI_MASTER)
        SetChipSelect();
    else
    {
        while(!writeEnable)
        {
            writeEnable = ReadChipSelect();
            if(tm.GetElapsedTimeInMS() >= timeoutInMs)
                return MemoryErrorHandling::MEM_HAL_TIMEOUT;
        }
    }
#if STM32
        ret = MemoryErrorHandling::HAL_StatusTypeDefToErr(
                HAL_SPI_Transmit(&m_SPIHandle->m_SPIHandle, data, *size, timeoutInMs));
#else
  //  m_SPIHandle.StoreBuffer(data, size); TODO
#endif // STM32
    if(m_SPIHandle->m_Mode == SPI_MASTER)
        ResetChipSelect();
    return ret;
}

/**
 * @brief Receives data on the specified SPI interface. CS must be set manually!
 * @param data byte buffer which is returned.
 * @param size the input specifies the maximum size of the read buffer. The same buffer is used to set the output size.
 * @param timeoutInMs timeoutInMs in ms when using blocking functions.
 * @return MEM_NO_ERROR if execution was successful.
 */
MEM_ERROR SPIWrapper::ReceiveData(uint8_t *data, uint16_t *size, BlockingMode blockingMode, uint32_t timeoutInMs)
{
    if(!size || *size == 0)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    bool retCS = ReadChipSelect();
    TimeMeasurement tm{};
    uint32_t timeStampStart = tm.ResetAndStartTimer();
    while(!retCS && tm.GetElapsedTimeInMS() - timeStampStart < timeoutInMs)
    {
        retCS = ReadChipSelect();
    }


    MEM_ERROR ret;
    if(m_SPIHandle->m_Mode == SPI_MASTER)
        SetChipSelect();
    else
    {
        bool readEnable = false;
        TimeMeasurement tm{};
        uint32_t timeStampStart = tm.ResetAndStartTimer();
        while(!readEnable)
        {
            readEnable = ReadChipSelect();
            if(tm.GetElapsedTimeInMS() >= timeoutInMs)
                return MemoryErrorHandling::MEM_HAL_TIMEOUT;
        }
    }

#if STM32
        ret = MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_SPI_Receive(&m_SPIHandle->m_SPIHandle, data, *size, timeoutInMs));
#else
    //m_SPIHandle.ReadBuffer(data, size); TODO
    ret = MemoryErrorHandling::MEM_NO_ERROR;
#endif // STM32
    if(m_SPIHandle->m_Mode == SPI_MASTER)
        ResetChipSelect();
    return ret;
}

/**
 * @brief This function reads the Chip Select signal if the the interface is used in slave mode.
 * @return Returns the value of the GPIO pin.
 */
inline bool SPIWrapper::ReadChipSelect()
{
#if STM32
    return  HAL_GPIO_ReadPin(SPI5_CS_GPIO_Port, SPI5_CS_Pin) == GPIO_PIN_RESET;
#endif // STM32
    return false; // TODO
}

#if UNIT_TEST
SPIWrapper::SPIWrapper(TestInterfaceWrapper& interfaceWrapper)
{

}
#endif // UNIT_TEST

