/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/MemoryControllers/MemoryControllerParallel.h"
#include "Logger.h"  // Include the Logger header for logging functionality



/*-- SRAM GPIOs Configuration --------------------------------------------------*/
/*
+-------------------+------------------+-------------------+
| PD14 <-> FMC_D0   | PF0  <-> FMC_A0  | PE0  <-> FMC_NBL0 |
| PD15 <-> FMC_D1   | PF1  <-> FMC_A1  | PE1  <-> FMC_NBL1 |
| PD0  <-> FMC_D2   | PF2  <-> FMC_A2  | PD4  <-> FMC_NOE  |
| PD1  <-> FMC_D3   | PF3  <-> FMC_A3  | PD5  <-> FMC_NWE  |
| PE7  <-> FMC_D4   | PF4  <-> FMC_A4  | PD7  <-> FMC_NE1  |
| PE8  <-> FMC_D5   | PF5  <-> FMC_A5  |-------------------+
| PE9  <-> FMC_D6   | PF12 <-> FMC_A6  |
| PE10 <-> FMC_D7   | PF13 <-> FMC_A7  |
| PE11 <-> FMC_D8   | PF14 <-> FMC_A8  |
| PE12 <-> FMC_D9   | PF15 <-> FMC_A9  |
| PE13 <-> FMC_D10  | PG0  <-> FMC_A10 |
| PE14 <-> FMC_D11  | PG1  <-> FMC_A11 |
| PE15 <-> FMC_D12  | PG2  <-> FMC_A12 |
| PD8  <-> FMC_D13  | PG3  <-> FMC_A13 |
| PD9  <-> FMC_D14  | PG4  <-> FMC_A14 |
| PD10 <-> FMC_D15  | PG5  <-> FMC_A15 |
+-------------------| PD11 <-> FMC_A16 |
                    | PD12 <-> FMC_A17 |
                    | PD13 <-> FMC_A18 |
                    | PE3  <-> FMC_A19 |
                    +------------------+
*/

#ifdef STM32
extern "C" {
#include <SystemFiles/fmc.h>
}
#endif // STM32

MemoryControllerParallel::MemoryControllerParallel(InterfaceWrapper *interfaceWrapper, MemoryModule &memoryModule,
                                                   DeviceWrapper &deviceWrapper)
        : MemoryController(deviceWrapper, interfaceWrapper, memoryModule)
{
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "MemoryControllerParallel initialized.");
}

MEM_ERROR MemoryControllerParallel::Initialize()
{
    auto initializePins =  [this] (const std::vector<GPIOPin>& gpioList) -> MEM_ERROR {
        MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
        for (const auto& pin : gpioList)
        {
            ret = m_DeviceWrapper.InitializeGPIOPin(pin, GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                                    {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
            if (ret != MemoryErrorHandling::MEM_NO_ERROR)
                return ret;
        }
        return ret;
    };

    MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing address lines.");
    ret = initializePins(m_Properties.GetAddressLinesList());
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize address lines.");
        return ret;
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing data lines.");
    ret = initializePins(m_Properties.GetDataLinesList());
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize data lines.");
        return ret;
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing other pins (WE, CS, LB, UB, OE).");
    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetWE(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize WE pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetCS(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize CS pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetLB(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize LB pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetUB(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize UB pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetOE(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET, {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if(ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize OE pin.");
        return ret;
    }


    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "MemoryControllerParallel initialized successfully.");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerParallel::SetTimingParameters(std::map<std::string, uint16_t> &timingParameters){
    if(m_initialized){
        Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Deinitialize SRAM controller");
        HAL_SRAM_DeInit(&hsram1);
    }
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initialize SRAM controller with new timing parameters");
#if STM32
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing FMC (STM32).");

    auto getTimingParameter = [&timingParameters](const char* value){
        auto pos = timingParameters.find(value);
        if (pos != timingParameters.end()) {
            return static_cast<int>(pos->second);
        } else {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Parameter %s not found!", value);
            return -1;
        }
    };

    int addressSetupTime =  getTimingParameter("addressSetupTime");
    if(addressSetupTime == -1)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    int addressHoldTime =  getTimingParameter("addressHoldTime");
    if(addressHoldTime == -1)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    int dataSetupTime =  getTimingParameter("dataSetupTime");
    if(dataSetupTime == -1)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    int busTurnAroundDuration =  getTimingParameter("busTurnAroundDuration");
    if(busTurnAroundDuration == -1)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    int clkDivision =  getTimingParameter("clkDivision");
    if(clkDivision == -1)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    int dataLatency = getTimingParameter("clkDivision");
    if (dataLatency == -1)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Configure FMC with following parameters (addressSetupTime: 0x%0x, addressHoldTime: 0x%0x, "
                "dataSetupTime: 0x%0x, busTurnAroundDuration: 0x%0x, clkDivision: 0x%0x, dataLatency: 0x%0x)",
                addressSetupTime, addressHoldTime, dataSetupTime, busTurnAroundDuration, clkDivision, dataLatency);

    MX_FMC_Init(addressSetupTime, addressHoldTime, dataSetupTime, busTurnAroundDuration, clkDivision, dataLatency);
    m_initialized = true;
#endif // STM32
    m_initialized = true;
    return MEM_ERROR::MEM_NO_ERROR;
}

/**
 * @brief This function writes an 8 bit value to the passed address.
 * Currently only a STM32F429DISC1 board is supported, which writes values using memory mapped IO.
 * @param adr address to write the value.
 * @param value the value to write to the specified address.
 * @return MEM_NO_ERROR if write execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Write8BitWord(uint32_t adr, uint8_t value)
{
    //Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Writing 8-bit value to address: 0x%08X", adr);

    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid address: 0x%08X", adr);
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;
    }

#if STM32
    *(__IO uint8_t *) (m_MMIOStartAddress + adr) = value;
    //Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Written 8-bit value 0x%02X to address 0x%08X", value, adr);
#endif // STM32
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function reads an 8-bit value from a passed address.
 * Currently only a STM32F429DISC1 board is supported, which reads values using memory mapped IO.
 * @param adr address to read from.
 * @param ret the value which is returned after the reading operation.
 * @return MEM_NO_ERROR if read execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Read8BitWord(uint32_t adr, uint8_t *ret)
{
//    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Reading 8-bit value from address: 0x%08X", adr);

    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid address: 0x%08X", adr);
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;
    }

    if(!ret) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid argument: null pointer for return value.");
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;
    }

#if STM32
    *ret = *(__IO uint8_t *) (m_MMIOStartAddress + adr);
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Read 8-bit value 0x%02X from address 0x%08X", *ret, adr);
#endif // STM32

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function writes a 16-bit value to the passed address.
 * Currently only a STM32F429DISC1 board is supported, which writes values using memory mapped IO.
 * @param adr address to write the value.
 * @param value the value to write to the specified address.
 * @return MEM_NO_ERROR if write execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Write16BitWord(uint32_t adr, uint16_t value)
{
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Writing 16-bit value to address: 0x%08X", adr);

    if(IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid address: 0x%08X", adr);
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;
    }

    // multiply address by 2 due to 2-byte-access (address is given as one byte)
    adr = adr << 1;

#if STM32
    *(__IO uint16_t *) (MEMORY_BANK_ADDRESS + adr) = value;
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Written 16-bit value 0x%04X to address 0x%08X", value, adr);
#endif // STM32

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function reads a 16-bit value from a passed address.
 * Currently only a STM32F429DISC1 board is supported, which reads values using memory mapped IO.
 * @param adr address to read from.
 * @param value the value to be read. Add a pointer as argument to retrieve the data.
 * @return MEM_NO_ERROR if read execution was successful. An error is returned if the address is invalid.
 */
MEM_ERROR MemoryControllerParallel::Read16BitWord(uint32_t adr, uint16_t *value)
{
    //Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Reading 16-bit value from address: 0x%08X", adr);

    if(IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid address: 0x%08X", adr);
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;
    }

    if(!value) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid argument: null pointer for return value.");
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;
    }

    // multiply address by 2 due to 2-byte-access (address is given as one byte)
    adr = adr << 1;

#if STM32
    *value = *(__IO uint16_t *) (m_MMIOStartAddress + adr);
    //Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Read 16-bit value 0x%04X from address 0x%08X", *value, adr);
#endif // STM32

    return MemoryErrorHandling::MEM_NO_ERROR;
}
