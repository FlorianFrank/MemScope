/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifdef STM32
#include "logger.h"
#include "cpp/Devices/STM32F429Wrapper.h"

#include "io_pin_defines.h"
#include <algorithm> // std::find

#define CLOCK_DEFAULT_SETTINGS 1

#if STM32
extern "C" {
#if RDMON_SPECS
extern void initialise_monitor_handles();
#endif // RDMON_SPECS
}
#endif // STM32

STM32F429Wrapper::~STM32F429Wrapper()
{
   DeInitialize();
}

/**
 * @brief This function initializes the clock configuration.
 * @return Error Code
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::SystemClockConfig()
{
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Initializing system clock...");
#if CLOCK_DEFAULT_SETTINGS
    // Configuring the internal regulator voltage
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Configuring internal regulator voltage...");
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    // Configuring RCC Oscillators
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Configuring RCC Oscillators...");
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Enable USART1 clock
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Enabling USART1 clock...");
    __HAL_RCC_USART1_CLK_ENABLE();

    // PLL Configuration
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Starting PLL configuration...");
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = PLL_FREQUENCY_MHZ;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 3;
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "PLL settings: PLLM=%d, PLLN=%d, PLLP=%d, PLLQ=%d", RCC_OscInitStruct.PLL.PLLM, RCC_OscInitStruct.PLL.PLLN, RCC_OscInitStruct.PLL.PLLP, RCC_OscInitStruct.PLL.PLLQ);

    HAL_StatusTypeDef ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if(ret != HAL_OK)
    {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "HAL_RCC_OscConfig failed with error code: %d", ret);
        return MemoryErrorHandling::HAL_StatusTypeDefToErr(ret);
    }
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "PLL configuration completed successfully.");

    // Clock Initialization
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing clocks...");
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
    if(ret != HAL_OK)
    {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "HAL_RCC_ClockConfig failed with error code: %d", ret);
        return MemoryErrorHandling::HAL_StatusTypeDefToErr(ret);
    }
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "System clock initialized successfully.");
#else
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Using alternate clock configuration...");
    // Similar detailed log messages for the alternate configuration...
#endif
    return MemoryErrorHandling::MEM_NO_ERROR;
}


/**
 * @brief This functions initializes the default GPIO settings and the Hardware Abstraction Layer of the STM32F429 board and sets the clock configuration.
 * @return MEM_NO_ERROR if no error occurred.
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::Initialize()
{
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "Initializing STM32F429Wrapper...");

#if RDMON_SPECS
    initialise_monitor_handles();
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "RDMON monitor handles initialized.");
#endif // RDMON_SPECS

    MemoryErrorHandling::MEM_ERROR ret = MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_Init());
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
    {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "HAL_Init failed with error code: %d", ret);
        return ret;
    }
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "HAL initialized successfully.");

    ret = InitializeDefaultGPIOSettings();
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
    {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "GPIO initialization failed with error code: %d", ret);
        return ret;
    }
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "GPIO initialized successfully.");

    ret = SystemClockConfig();
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
    {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "System clock configuration failed with error code: %d", ret);
        return ret;
    }
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "System clock configured successfully.");

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function de-initializes the initialized GPIO pins, de-Initializes the
 * Hardware Abstraction Layer as well as the system clock
 * @return
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitialize()
{
    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "De-initializing STM32F429Wrapper...");

    while(!m_InitializedGPIOPins.empty())
    {
        GPIOPin pin = m_InitializedGPIOPins.back();
        Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "De-initializing GPIO pin: %d" + pin.GetGPIOPin());
        auto ret = DeInitializeGPIOPin(pin);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        {
            Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "De-initializing GPIO pin failed with error code: %d" + ret);
            return ret;
        }
    }

    while(!m_InitializedGPIOBanks.empty())
    {
        GPIOBank bank = m_InitializedGPIOBanks.back();
        Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "De-initializing GPIO bank: %d", bank);
        auto ret = DeInitializeGPIOBank(bank);
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        {
            Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "De-initializing GPIO bank failed with error code: %d" + ret);
            return ret;
        }
    }

    MemoryErrorHandling::MEM_ERROR err = MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_DeInit());
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
    {
        Logger::log(LogLevel::ERROR, __FILE_NAME__, __LINE__, "HAL_DeInit failed with error code: %d", err);
        return err;
    }

    Logger::log(LogLevel::INFO, __FILE_NAME__, __LINE__, "HAL de-initialized successfully.");

    // Disable SYS_CLK
    __HAL_RCC_PWR_CLK_DISABLE();
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "SYS_CLK disabled.");

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This method initializes the GPIO Pins required by SPI4.
 * @return MEM_NO_ERROR if no error occurred
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeSPI4()
{
    __HAL_RCC_SPI4_CLK_ENABLE();
    return InitializeGPIOPinDeviceSpecific({GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6}, GPIO_PIN_SET, GPIO_MODE_AF_PP, GPIO_NOPULL,
                      GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI4);
}

/**
 * @brief This method initializes the GPIO Pins required by SPI5.
 * @return MEM_NO_ERROR if no error occurred
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeSPI5()
{
    __HAL_RCC_SPI5_CLK_ENABLE();
    MemoryErrorHandling::MEM_ERROR  ret = InitializeGPIOPinDeviceSpecific({GPIOF, SPI5_CS_Pin | SPI5_HOLDspi5_Pin}, GPIO_PIN_SET, GPIO_MODE_INPUT, GPIO_NOPULL,
                      GPIO_SPEED_FREQ_VERY_HIGH, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    return InitializeGPIOPinDeviceSpecific({GPIOF, GPIO_PIN_7 | SPI5_MISO_Pin | SPI5_MOSI_Pin}, GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL,
                      GPIO_SPEED_FREQ_LOW, GPIO_AF5_SPI5);
}

/**
 * @brief This method de-initializes the GPIO Pins required by SPI4.
 * @return MEM_NO_ERROR if no error occurred
 */
__attribute__((unused)) MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeSPI4()
{
    __HAL_RCC_SPI4_CLK_DISABLE();
    return DeInitializeGPIOPin({GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6});
}

/**
 * @brief This method de-initializes the GPIO Pins required by SPI5.
 * @return MEM_NO_ERROR if no error occurred
 */
__attribute__((unused)) MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeSPI5()
{
    __HAL_RCC_SPI5_CLK_DISABLE();
    return DeInitializeGPIOPin({GPIOF, GPIO_PIN_7 | SPI5_MISO_Pin | SPI5_MOSI_Pin | SPI5_CS_Pin | SPI5_HOLDspi5_Pin});
}

/**
 * @brief This method initializes a GPIO pin and also the corresponding bank if needed.
 * @return MEM_NO_ERROR if no error occurred otherwise MEM_GPIO_PIN_ALREADY_IN_USE or MEM_INVALID_ARGUMENT is returned.
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeGPIOPinDeviceSpecific(const GPIOPin& gpioPin, GPIO_PinState gpioPinState, uint32_t mode, uint32_t pull,
                                                                   uint32_t speed, uint32_t alternate)
{
    GPIOBank bank = gpioPin.GetGPIOBank();
    if (!bank)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    if(std::find(m_InitializedGPIOBanks.begin(), m_InitializedGPIOBanks.end(), gpioPin.GetGPIOBank()) == m_InitializedGPIOBanks.end())
    {
        MemoryErrorHandling::MEM_ERROR ret = InitializeGPIOBank(gpioPin.GetGPIOBank());
        if(ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        m_InitializedGPIOBanks.push_back(bank);
    }

    if(std::find(m_InitializedGPIOPins.begin(), m_InitializedGPIOPins.end(), gpioPin) != m_InitializedGPIOPins.end())
        return MemoryErrorHandling::MEM_GPIO_PIN_ALREADY_IN_USE;

    uint32_t pin = gpioPin.GetGPIOPin();
    GPIO_InitTypeDef gpioInitStruct = {0};
    gpioInitStruct.Pin = pin;
    gpioInitStruct.Mode = mode;
    gpioInitStruct.Pull = pull;
    gpioInitStruct.Speed = speed;
    if(alternate != IO_PIN_UNDEFINED)
        gpioInitStruct.Alternate = alternate;

    HAL_GPIO_Init(bank, &gpioInitStruct);
    HAL_GPIO_WritePin(bank, pin, gpioPinState);
    m_InitializedGPIOPins.push_back(gpioPin);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This method de-initializes the passed GPIO pin.
 * @param gpioPin pin to de-initialize
 * @return MEM_NO_ERROR is returned if no error occurred.
 * Internal Error is returned if the pin could not be fined in the list of initialized Pins.
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeGPIOPin(const GPIOPin &gpioPin)
{
    Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Deinitializing GPIO pin.");
    HAL_GPIO_DeInit(gpioPin.GetGPIOBank(), gpioPin.GetGPIOPin());

    auto it = std::find(m_InitializedGPIOPins.begin(), m_InitializedGPIOPins.end(), gpioPin);
    if (it != m_InitializedGPIOPins.end()) {
        m_InitializedGPIOPins.remove(gpioPin);
        Logger::log(LogLevel::INFO, __FILE__, __LINE__, "GPIO pin deinitialized successfully.");
    } else {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Failed to deinitialize GPIO pin: not found in initialized list.");
        return MemoryErrorHandling::MEM_HAL_INTERNAL_ERROR;
    }

    m_InitializedGPIOPins.erase(std::remove(m_InitializedGPIOPins.begin(), m_InitializedGPIOPins.end(), gpioPin),
                                m_InitializedGPIOPins.end());

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief
 * @param gpioBank
 * @return
 */
__attribute__((unused)) MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeGPIOBank(GPIOBank gpioBank)
{
    if (gpioBank == IO_BANK_A)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (gpioBank == IO_BANK_B)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (gpioBank == IO_BANK_C)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (gpioBank == IO_BANK_D)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (gpioBank == IO_BANK_E)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (gpioBank == IO_BANK_F)
        __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (gpioBank == IO_BANK_G)
        __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (gpioBank == IO_BANK_H)
        __HAL_RCC_GPIOH_CLK_ENABLE();
    else if (gpioBank == IO_BANK_I)
        __HAL_RCC_GPIOI_CLK_ENABLE();
    else if (gpioBank == IO_BANK_UNDEFINED)
        return MemoryErrorHandling::MEM_INTERFACE_NOT_FOUND;
    else
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    m_InitializedGPIOBanks.push_back(gpioBank);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function disables the clock for the passed bank and removes it from the list of initialized banks.
 * @param gpioBank bank of which the clock should be disabled and which should be removed from the list.
 * @return MEM_NO_ERROR if no error occurs otherwise MEM_INTERFACE_NOT_FOUND when the bank is IO_BANK_UNDEFINED,
 * MEM_INVALID_ARGUMENT if the bank could not be found.
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeGPIOBank(GPIOBank gpioBank)
{
    if (gpioBank == IO_BANK_A)
        __HAL_RCC_GPIOA_CLK_DISABLE();
    else if (gpioBank == IO_BANK_B)
        __HAL_RCC_GPIOB_CLK_DISABLE();
    else if (gpioBank == IO_BANK_C)
        __HAL_RCC_GPIOC_CLK_DISABLE();
    else if (gpioBank == IO_BANK_D)
        __HAL_RCC_GPIOD_CLK_DISABLE();
    else if (gpioBank == IO_BANK_E)
        __HAL_RCC_GPIOE_CLK_DISABLE();
    else if (gpioBank == IO_BANK_F)
        __HAL_RCC_GPIOF_CLK_DISABLE();
    else if (gpioBank == IO_BANK_G)
        __HAL_RCC_GPIOG_CLK_DISABLE();
    else if (gpioBank == IO_BANK_H)
        __HAL_RCC_GPIOH_CLK_DISABLE();
    else if (gpioBank == IO_BANK_I)
        __HAL_RCC_GPIOI_CLK_DISABLE();
    else if (gpioBank == IO_BANK_UNDEFINED)
        return MemoryErrorHandling::MEM_INTERFACE_NOT_FOUND;
    else
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    m_InitializedGPIOBanks.erase(std::remove(m_InitializedGPIOBanks.begin(), m_InitializedGPIOBanks.end(), gpioBank),
                                 m_InitializedGPIOBanks.end());

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Overloaded function initializes a hardware interface like SPI1 or UART5 on the STM board.
 * @param interfaceName name of the interface as string to allow abstraction from device specific interface handles.
 * @return MEM_NO_ERROR if no error occurs otherwise return error code.
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeHardwareInterface(std::string &interfaceName)
{
    if (interfaceName == "SPI1")
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
    if (interfaceName == "SPI2")
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
    if (interfaceName == "SPI3")
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;
    if (interfaceName == "SPI4")
        return InitializeSPI4();
    if (interfaceName == "SPI5")
        return InitializeSPI5();
    if (interfaceName == "SPI6")
        return MemoryErrorHandling::MEM_INTERFACE_NOT_SUPPORTED;

    return MemoryErrorHandling::MEM_INTERFACE_NOT_FOUND;
}

/**
 * @brief This method initializes the device with default GPIO settings. Sets the required clocks, enables STLink or RESET pins.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeDefaultGPIOSettings()
{
    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing USART1 GPIOs");
    MemoryErrorHandling::MEM_ERROR ret = InitializeGPIOPinDeviceSpecific({IO_BANK_A, STLINK_RX_Pin|STLINK_TX_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF7_USART1);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing B1, MEMS INT, and TP INT GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_A, B1_Pin|MEMS_INT1_Pin|MEMS_INT2_Pin|TP_INT1_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_EVT_RISING, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing ACP_RST GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_A, ACP_RST_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing BOOT1 GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_B, BOOT1_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing G4, G5, B6, B7, ZZ GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_B, G4_Pin|G5_Pin|B6_Pin|B7_Pin | ZZ_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing SPI4 WP, NCS MEMS SPI, CSX, OTG FS PSO GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_C, SPI4_WP_Pin|NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing OTG FS OC GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_C, OTG_FS_OC_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_EVT_RISING, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing G6 GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_C, G6_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing I2C3 SDA GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_C, I2C3_SDA_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF4_I2C3);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing I2C3 SCL GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_C, I2C3_SCL_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF4_I2C3);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing SPI5 WP GPIO");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_C, SPI5_WP_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing G7, B2 GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_D, G7_Pin|B2_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing IO_BANK_E");
    ret = InitializeGPIOBank(IO_BANK_E);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing SPI5 CS and SPI5 HOLD GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_F, SPI5_CS_Pin|SPI5_HOLDspi5_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing R7, DOTCLK, B3 GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_G, R7_Pin|DOTCLK_Pin|B3_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing G3, B4 GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_G, G3_Pin|B4_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF9_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing LD3, LD4 GPIOs");
    ret = InitializeGPIOPinDeviceSpecific({IO_BANK_G, LD3_Pin|LD4_Pin},
                   GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF9_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    Logger::log(LogLevel::DEBUG, __FILE_NAME__, __LINE__, "Initializing IO_BANK_H");
    ret = InitializeGPIOBank(IO_BANK_H);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    HAL_GPIO_WritePin(IO_BANK_F, SPI5_CS_Pin|SPI5_HOLDspi5_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IO_BANK_C, SPI5_WP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IO_BANK_B, ZZ_Pin, GPIO_PIN_SET);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR
STM32F429Wrapper::InitializeGPIOPin(GPIOPin pin, GPIOMode mode, GPIOState initialState, GPIOPin alternate)
{
    int stmMode;
    switch (mode)
    {
        case GPIO_INPUT:
            stmMode = GPIO_MODE_INPUT;
            break;
        case GPIO_OUTPUT:
            stmMode = GPIO_MODE_OUTPUT_PP;
            break;
        case GPIO_OUTPUT_PUSH_PULL:
            stmMode = GPIO_MODE_OUTPUT_PP;
            break;
        case GPIO_OUTPUT_OPEN_DRAIN:
            stmMode = GPIO_MODE_OUTPUT_OD;
            break;
        case GPIO_ALTERNATE_PUSH_PULL:
            stmMode = GPIO_MODE_AF_PP;
            break;
        case GPIO_ALTERNATE_OPEN_DRAIN:
            stmMode = GPIO_MODE_AF_OD;
            break;
        default:
            stmMode = GPIO_MODE_OUTPUT_PP;

    }


   return InitializeGPIOPinDeviceSpecific(pin,
                                          (initialState == GPIO_RESET) ? GPIO_PIN_RESET : GPIO_PIN_SET, stmMode, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, alternate.GetGPIOPin());
}


#endif // STM32