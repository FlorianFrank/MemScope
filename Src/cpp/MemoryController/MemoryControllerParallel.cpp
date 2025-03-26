/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/MemoryControllers/MemoryControllerParallel.h"
#include "Logger.h"  // Include the Logger header for logging functionality

#include <cmath>

#include <cstring>

/*-- SRAM GPIOs Configuration --------------------------------------------------*/
/*
+-------------------+------------------+-------------------+
| PD14 <-> FMC_D0   | PF0  <-> FMC_A0  | PE0  <-> FMC_NBL0 |
| PD15 <-> FMC_D1   | PF1  <-> FMC_A1  | PE1  <-> FMC_NBL1 |
| PD0  <-> FMC_D2   | PF2  <-> FMC_A2  | PD4  <-> FMC_OE  |
| PD1  <-> FMC_D3   | PF3  <-> FMC_A3  | PD5  <-> FMC_WE  |
| PE7  <-> FMC_D4   | PF4  <-> FMC_A4  | PG9  <-> FMC_CE  |
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

constexpr float PLL_FREQUENCY_CONFIG_PARAM = 400.0;

#ifdef STM32
extern "C" {
#include <SystemFiles/fmc.h>
}
#include "cpp/Devices/STM32F429Wrapper.h"
#endif // STM32

MemoryControllerParallel::MemoryControllerParallel(InterfaceWrapper *interfaceWrapper, MemoryModule &memoryModule,
                                                   DeviceWrapper &deviceWrapper)
        : MemoryController(deviceWrapper, interfaceWrapper, memoryModule) {
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "MemoryControllerParallel initialized.");
}

MEM_ERROR MemoryControllerParallel::Initialize() {
    if(m_initialized)
        return MemoryErrorHandling::MEM_NO_ERROR;
    auto initializePins = [this](const std::vector<GPIOPin> &gpioList) -> MEM_ERROR {
        MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
        for (const auto &pin: gpioList) {
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
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize address lines.");
        return ret;
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing data lines.");
    ret = initializePins(m_Properties.GetDataLinesList());
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize data lines.");
        return ret;
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing other pins (WE, CS, LB, UB, OE).");
    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetWE(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                            {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize WE pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetCS(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                            {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize CS pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetLB(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                            {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize LB pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetUB(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                            {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize UB pin.");
        return ret;
    }

    ret = m_DeviceWrapper.InitializeGPIOPin(m_Properties.GetOE(), GPIO_ALTERNATE_PUSH_PULL, GPIO_RESET,
                                            {IO_BANK_UNDEFINED, IO_PIN_UNDEFINED});
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to initialize OE pin.");
        return ret;
    }


    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "MemoryControllerParallel initialized successfully.");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerParallel::Deinitialize() {
    auto deinitializePins = [this](const std::vector<GPIOPin> &gpioList) -> MEM_ERROR {
        MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
        for (const auto &pin: gpioList) {
            ret = m_DeviceWrapper.DeInitializeGPIOPin(pin);
            if (ret != MemoryErrorHandling::MEM_NO_ERROR)
                return ret;
        }
        return ret;
    };

    MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Deinitializing address lines.");
    ret = deinitializePins(m_Properties.GetAddressLinesList());
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to deinitialize address lines.");
        return ret;
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Deinitializing data lines.");
    ret = deinitializePins(m_Properties.GetDataLinesList());
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to deinitialize data lines.");
        return ret;
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Deinitializing other pins (WE, CS, LB, UB, OE).");
    std::vector<GPIOPin> controlPins = {m_Properties.GetWE(), m_Properties.GetCS(), m_Properties.GetLB(),
                                        m_Properties.GetUB(), m_Properties.GetOE()};
    for (const auto &pin : controlPins) {
        ret = m_DeviceWrapper.DeInitializeGPIOPin(pin);
        if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to deinitialize a control pin.");
            return ret;
        }
    }

    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "MemoryControllerParallel deinitialized successfully.");
    return MemoryErrorHandling::MEM_NO_ERROR;
}

extern SRAM_HandleTypeDef hsram1;


/**
 * @brief Sets the timing parameters for the SRAM controller.
 *
 * This function initializes the SRAM controller with the provided timing parameters.
 * It checks the validity of each parameter and logs any errors.
 *
 * @param timingParameters Map containing the timing parameters with their respective values:
 *   - "addressSetupTime": Number of HCLK cycles to configure the duration of the address setup time [0, 15].
 *   - "addressHoldTime": Number of HCLK cycles to configure the duration of the address hold time [1, 15].
 *   - "dataSetupTime": Number of HCLK cycles to configure the duration of the data setup time [1, 255].
 *   - "busTurnAroundDuration": Number of HCLK cycles to configure the duration of the bus turnaround [0, 15].
 *   - "clkDivision": Period of CLK clock output signal in HCLK cycles [2, 16].
 *   - "dataLatency": Number of memory clock cycles before getting the first data [0, 17].
 *   Measured steps by an oscilloscope on WE, by reducing the data setup time  75.9 (value = 9) -> 67.2 -> 59.4 -> 49.4 -> 41.6 -> 33.8 -> 24.7
 *
 * @return MEM_ERROR::MEM_NO_ERROR if successful, MEM_ERROR::MEM_INVALID_ARGUMENT if any parameter is invalid.
 */
MEM_ERROR MemoryControllerParallel::SetTimingParameters(std::map<std::string, uint16_t> &timingParameters) {
    if (m_initialized) {
        Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Deinitializing SRAM controller");

        auto deinitializeSRAM = [](SRAM_HandleTypeDef *hsram) {

            FMC_NORSRAM_DeInit(hsram->Instance, hsram->Extended, hsram->Init.NSBank);

            hsram->State = HAL_SRAM_STATE_RESET;

            /* Release Lock */
            __HAL_UNLOCK(hsram);

            return HAL_OK;
        };

        auto resetFMCClocks = [](){
            HAL_Delay(10);

            Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Forcing FMC reset");
            __HAL_RCC_FMC_FORCE_RESET();
            Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Releasing FMC reset");
            __HAL_RCC_FMC_RELEASE_RESET();
            Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Disabling FMC clock");
            __HAL_RCC_FMC_CLK_DISABLE();
            HAL_Delay(10);
            Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Enabling FMC clock");
            __HAL_RCC_FMC_CLK_ENABLE();
        };

        deinitializeSRAM(&hsram1);
        resetFMCClocks();
        HAL_SRAM_MspInit(&hsram1);
    }
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initialize SRAM controller with new timing parameters");
#if STM32
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Initializing FMC (STM32).");

    auto getTimingParameter = [&timingParameters](const char *value) {
        auto pos = timingParameters.find(value);
        if (pos != timingParameters.end()) {
            auto timeInNs = static_cast<float>(pos->second) * 2.5; // The config file is in a granularity of 2.5 ns based on the 400 MHz clock on the FPGA
            float periodInNs = 7.8; // We measured a clock phase of 7.8 for a PLL=120 MHz

            /**
             * Following timing is tested, for example for the CE line
             * ds = 9  CE Phase 75.9 ns -> a config param of 30 as 75/2.5 = 30 must result in ds = 9 thereby 75 / 7.8 =9.6 -> 9.
             * Always round down by casting to int.
             *
             * Measured following values with an oscilloscope
             * 9 = 75.9 ns
             * 8 = 67.2 ns
             * 7 = 59.2 ns
             * 6 = 50.3 ns
             * 5 = 41.6 ns
             * 4 = 33.6 ns
             * 3 = 25.6 ns
             * 2 = 16.7 ns
             * 1 =
             * 2 =
            */

            return static_cast<int>(timeInNs/periodInNs);
        } else {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Parameter %s not found!", value);
            return -1;
        }
    };

    auto checkParameterRange = [](int parameter, int minParam, int maxParam, const char *paramName) {
        if (parameter != -1 && parameter >= minParam && parameter <= maxParam) {
            return true;
        } else {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__,
                        "Setting %s failed as parameter %d is not within range [%d,%d]", paramName, parameter, minParam,
                        maxParam);
            return false;
        }
    };

    /* Defines the number of HCLK cycles to configure
     * the duration of the address setup time.
     * This parameter can be a value between Min_Data = 0 and Max_Data = 15.
     * @note This parameter is not used with synchronous NOR Flash memories.*/
    int addressSetupTime = getTimingParameter("addressSetupTime");
    if (!checkParameterRange(addressSetupTime, 0, 15, "addressSetupTime"))
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    /* Defines the number of HCLK cycles to configure
     * the duration of the address hold time.
     * This parameter can be a value between Min_Data = 1 and Max_Data = 15.
     * @note This parameter is not used with synchronous NOR Flash memories.*/
    int addressHoldTime = getTimingParameter("addressHoldTime");
    if (!checkParameterRange(addressHoldTime, 1, 15, "addressHoldTime"))
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    /* DataSetupTime Defines the number of HCLK cycles to configure
     * the duration of the data setup time.
     * This parameter can be a value between Min_Data = 1 and Max_Data = 255.
     * @note This parameter is used for SRAMs, ROMs and asynchronous multiplexed
     * NOR Flash memories.*/
    int dataSetupTime = getTimingParameter("dataSetupTime");
    if (!checkParameterRange(dataSetupTime, 1, 255, "dataSetupTime"))
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    /* BusTurnAroundDuration Defines the number of HCLK cycles to configure
     * the duration of the bus turnaround. This parameter can be a value between Min_Data = 0 and Max_Data = 15.
     * @note This parameter is only used for multiplexed NOR Flash memories. */
    int busTurnAroundDuration = getTimingParameter("busTurnAroundDuration");
    if (!checkParameterRange(busTurnAroundDuration, 0, 15, "busTurnAroundDuration"))
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    /* CLKDivision Defines the period of CLK clock output signal, expressed in number of
     * HCLK cycles. This parameter can be a value between Min_Data = 2 and Max_Data = 16.
     * @note This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses. */
    int clkDivision = getTimingParameter("clkDivision");
    if (!checkParameterRange(clkDivision, 2, 16, "clkDivision"))
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    /* DataLatency Defines the number of memory clock cycles to issue
     * to the memory before getting the first data.
     * The parameter value depends on the memory type as shown below:
     * - It must be set to 0 in case of a CRAM
     * - It is don't care in asynchronous NOR, SRAM or ROM accesses
     * - It may assume a value between Min_Data = 2 and Max_Data = 17 in NOR Flash memories
     * with synchronous burst mode enable */
    int dataLatency = getTimingParameter("dataLatency");
    if (!checkParameterRange(dataLatency, 0, 17, "dataLatency"))
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__,
                "Configure FMC with following parameters (addressSetupTime: 0x%0x, addressHoldTime: 0x%0x, "
                "dataSetupTime: 0x%0x, busTurnAroundDuration: 0x%0x, clkDivision: 0x%0x, dataLatency: 0x%0x)",
                addressSetupTime, addressHoldTime, dataSetupTime, busTurnAroundDuration, clkDivision, dataLatency);

    // Configure timing parameters
    FMC_NORSRAM_TimingTypeDef timing = {0};
    timing.AddressSetupTime = addressSetupTime;
    timing.AddressHoldTime = addressHoldTime;
    timing.DataSetupTime = dataSetupTime;
    timing.BusTurnAroundDuration = busTurnAroundDuration;
    timing.CLKDivision = clkDivision;
    timing.DataLatency = dataLatency;
    timing.AccessMode = FMC_ACCESS_MODE_A;


    hsram1.Instance = FMC_NORSRAM_DEVICE;
    hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
    hsram1.Init.NSBank = FMC_NORSRAM_BANK2;
    hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
    hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
    hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_8;
    hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
    hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram1.Init.WrapMode = FMC_WRAP_MODE_DISABLE;
    hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
    hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
    hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
    hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
    hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
    hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
    hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;

    if (HAL_SRAM_Init(&hsram1, &timing, NULL) != HAL_OK) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "HAL SRAM INIT FAILED");
    }
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "HAL_SRAM_Init called successfully");
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
MEM_ERROR MemoryControllerParallel::Write8BitWord(uint32_t adr, uint8_t value) {
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
MEM_ERROR MemoryControllerParallel::Read8BitWord(uint32_t adr, uint8_t *ret) {
//    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Reading 8-bit value from address: 0x%08X", adr);

    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid address: 0x%08X", adr);
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;
    }

    if (!ret) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid argument: null pointer for return value.");
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;
    }

#if STM32
    *ret = *(__IO uint8_t *) (m_MMIOStartAddress + adr);
    //Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Read 8-bit value 0x%02X from address 0x%08X", *ret, adr);
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
MEM_ERROR MemoryControllerParallel::Write16BitWord(uint32_t adr, uint16_t value) {
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Writing 16-bit value to address: 0x%08X", adr);

    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
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
MEM_ERROR MemoryControllerParallel::Read16BitWord(uint32_t adr, uint16_t *value) {
    //Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Reading 16-bit value from address: 0x%08X", adr);

    if (IsInvalidAddress(adr, m_MemoryModule.GetMemorySize())) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Invalid address: 0x%08X", adr);
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;
    }

    if (!value) {
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
