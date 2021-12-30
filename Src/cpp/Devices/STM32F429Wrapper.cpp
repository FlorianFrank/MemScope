#ifdef STM32

/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include <io_pin_defines.h>
#include <algorithm>
#include "cpp/Devices/STM32F429Wrapper.h"

#if STM32
extern "C" {
#include "error_handler.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_pwr_ex.h"
#include <SystemFiles/gpio.h>
#include <stm32f4xx_hal_rcc_ex.h>


#if RDMON_SPECS
extern void initialise_monitor_handles();
#endif // RDMON_SPECS
}
#endif // STM32

STM32F429Wrapper::STM32F429Wrapper(): DeviceWrapper(){}

STM32F429Wrapper::~STM32F429Wrapper()
{
    for (GPIOPin& elem: m_InitializedGPIOPins)
    {
        DeInitializeGPIOPin(elem);
    }
    for(GPIO_TypeDef* elem: m_InitializedGPIOBanks)
    {
        DeInitializeGPIOBank(elem);
    }
}

/**
 * @brief This function initializes the clock configuration.
 * @return Error Code
 */
/*static*/ MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::SystemClockConfig()
{
#if 1 // TODO
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

/** Configure the main internal regulator output voltage
*/
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
/** Initializes the RCC Oscillators according to the specified parameters
* in the RCC_OscInitTypeDef structure.
*/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 72;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 3;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();

    }
/** Initializes the CPU, AHB and APB buses clocks
*/
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();

    }
#else
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

        /** Configure the main internal regulator output voltage
        */
        __HAL_RCC_PWR_CLK_ENABLE();
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
        /** Initializes the RCC Oscillators according to the specified parameters
        * in the RCC_OscInitTypeDef structure.
        */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLM = 4;
        RCC_OscInitStruct.PLL.PLLN = 72;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
        RCC_OscInitStruct.PLL.PLLQ = 3;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            Error_Handler();
        }
        /** Initializes the CPU, AHB and APB buses clocks
        */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                      |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
        {
              Error_Handler();
        }
#endif
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::Initialize()
{
#if RDMON_SPECS
    initialise_monitor_handles();
#endif // RDMON_SPECS

    MemoryErrorHandling::MEM_ERROR ret = InitializeDefaultGPIOSettings();
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    return SystemClockConfig();
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitialize()
{
    MemoryErrorHandling::MEM_ERROR err = MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_DeInit());
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeSPI4()
{
    __HAL_RCC_SPI4_CLK_ENABLE();
    InitializeGPIOPin({GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6}, GPIO_PIN_SET, GPIO_MODE_AF_PP, GPIO_NOPULL,
                      GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI4);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeSPI5()
{
    __HAL_RCC_SPI5_CLK_ENABLE();
    InitializeGPIOPin({GPIOF, SPI5_CS_Pin | SPI5_HOLDspi5_Pin}, GPIO_PIN_SET, GPIO_MODE_INPUT, GPIO_NOPULL,
                      GPIO_SPEED_FREQ_VERY_HIGH, 0);
    InitializeGPIOPin({GPIOF, GPIO_PIN_7 | SPI5_MISO_Pin | SPI5_MOSI_Pin}, GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL,
                      GPIO_SPEED_FREQ_LOW, GPIO_AF5_SPI5);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

__attribute__((unused)) MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeSPI4()
{
    __HAL_RCC_SPI4_CLK_DISABLE();
    DeInitializeGPIOPin({GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6});
    return MemoryErrorHandling::MEM_REGISTER_NOT_SET;
}

__attribute__((unused)) MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeSPI5()
{
    __HAL_RCC_SPI5_CLK_DISABLE();
    return DeInitializeGPIOPin({GPIOF, GPIO_PIN_7 | SPI5_MISO_Pin | SPI5_MOSI_Pin | SPI5_CS_Pin | SPI5_HOLDspi5_Pin});
}

MemoryErrorHandling::MEM_ERROR
STM32F429Wrapper::InitializeGPIOPin(GPIOPin gpioPin, GPIO_PinState gpioPinState, uint32_t mode, uint32_t pull,
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

    uint32_t pin = gpioPin.GetGPIOPin();
    GPIO_InitTypeDef gpioInitStruct = {0};
    gpioInitStruct.Pin = pin;
    gpioInitStruct.Mode = mode;
    gpioInitStruct.Pull = pull;
    gpioInitStruct.Speed = speed;
    gpioInitStruct.Alternate = alternate;

    HAL_GPIO_Init(bank, &gpioInitStruct);
    HAL_GPIO_WritePin(bank, pin, gpioPinState);
    m_InitializedGPIOPins.push_back(gpioPin);

    return MemoryErrorHandling::MEM_NO_ERROR;
}


MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeGPIOPin(const GPIOPin &gpioPin)
{
    HAL_GPIO_DeInit(gpioPin.GetGPIOBank(), gpioPin.GetGPIOPin());
    m_InitializedGPIOPins.remove(gpioPin);
    return MemoryErrorHandling::MEM_REGISTER_NOT_SET;
}

__attribute__((unused)) MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeGPIOBank(GPIOBank gpioBank)
{
    if (gpioBank == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (gpioBank == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (gpioBank == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (gpioBank == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (gpioBank == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (gpioBank == GPIOF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (gpioBank == GPIOG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (gpioBank == GPIOH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
    else if (gpioBank == GPIOI)
        __HAL_RCC_GPIOI_CLK_ENABLE();
    else
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    m_InitializedGPIOBanks.push_back(gpioBank);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeGPIOBank(GPIOBank GPIOBank)
{
    if (GPIOBank == GPIOA)
        __HAL_RCC_GPIOA_CLK_DISABLE();
    else if (GPIOBank == GPIOB)
        __HAL_RCC_GPIOB_CLK_DISABLE();
    else if (GPIOBank == GPIOC)
        __HAL_RCC_GPIOC_CLK_DISABLE();
    else if (GPIOBank == GPIOD)
        __HAL_RCC_GPIOD_CLK_DISABLE();
    else if (GPIOBank == GPIOE)
        __HAL_RCC_GPIOE_CLK_DISABLE();
    else if (GPIOBank == GPIOF)
        __HAL_RCC_GPIOF_CLK_DISABLE();
    else if (GPIOBank == GPIOG)
        __HAL_RCC_GPIOG_CLK_DISABLE();
    else if (GPIOBank == GPIOH)
        __HAL_RCC_GPIOH_CLK_DISABLE();
    else if (GPIOBank == GPIOI)
        __HAL_RCC_GPIOI_CLK_DISABLE();
    else
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    m_InitializedGPIOBanks.erase(std::remove(m_InitializedGPIOBanks.begin(), m_InitializedGPIOBanks.end(), GPIOBank),
                                 m_InitializedGPIOBanks.end());

    return MemoryErrorHandling::MEM_NO_ERROR;
}

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

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeDefaultGPIOSettings()
{

    MemoryErrorHandling::MEM_ERROR ret = MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_Init());
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_A, STLINK_RX_Pin|STLINK_TX_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF7_USART1);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;


    ret = InitializeGPIOPin({IO_BANK_A, B1_Pin|MEMS_INT1_Pin|MEMS_INT2_Pin|TP_INT1_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_EVT_RISING, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_A, ACP_RST_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_B, BOOT1_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_B, G4_Pin|G5_Pin|B6_Pin|B7_Pin | ZZ_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_C, SPI4_WP_Pin|NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_C, OTG_FS_OC_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_EVT_RISING, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_C, G6_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_C, I2C3_SDA_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF4_I2C3);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_C, I2C3_SCL_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, GPIO_AF4_I2C3);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_C, SPI5_WP_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;


    ret = InitializeGPIOPin({IO_BANK_D, G7_Pin|B2_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOBank(IO_BANK_E);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_F, SPI5_CS_Pin|SPI5_HOLDspi5_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_G, R7_Pin|DOTCLK_Pin|B3_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF14_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_G, G3_Pin|B4_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF9_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOPin({IO_BANK_G, LD3_Pin|LD4_Pin},
                            GPIO_PIN_RESET, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF9_LTDC);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    ret = InitializeGPIOBank(IO_BANK_H);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    HAL_GPIO_WritePin(IO_BANK_F, SPI5_CS_Pin|SPI5_HOLDspi5_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IO_BANK_C, SPI5_WP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(IO_BANK_B, ZZ_Pin, GPIO_PIN_SET);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

#endif // STM32
