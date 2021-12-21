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
        DeInitializeGPIO(elem);
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
/*static*/ MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::SystemClock_Config()
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

    MemoryErrorHandling::MEM_ERROR err = MemoryErrorHandling::HAL_StatusTypeDefToErr(HAL_Init());
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    MX_GPIO_Init(); // TODO remove

    return SystemClock_Config();
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
    InitializeGPIO({GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6}, GPIO_PIN_SET, GPIO_MODE_AF_PP, GPIO_NOPULL,
                   GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF5_SPI4);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeSPI5()
{
    __HAL_RCC_SPI5_CLK_ENABLE();
    InitializeGPIO({GPIOF, SPI5_CS_Pin | SPI5_HOLDspi5_Pin}, GPIO_PIN_SET, GPIO_MODE_INPUT, GPIO_NOPULL,
                   GPIO_SPEED_FREQ_VERY_HIGH, 0);
    InitializeGPIO({GPIOF, GPIO_PIN_7 | SPI5_MISO_Pin | SPI5_MOSI_Pin}, GPIO_PIN_RESET, GPIO_MODE_AF_PP, GPIO_NOPULL,
                   GPIO_SPEED_FREQ_LOW, GPIO_AF5_SPI5);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeSPI4()
{
    __HAL_RCC_SPI4_CLK_DISABLE();
    DeInitializeGPIO({GPIOE, GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6});
    return MemoryErrorHandling::MEM_REGISTER_NOT_SET;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeSPI5()
{
    __HAL_RCC_SPI5_CLK_DISABLE();
    return DeInitializeGPIO({GPIOF, GPIO_PIN_7 | SPI5_MISO_Pin | SPI5_MOSI_Pin | SPI5_CS_Pin | SPI5_HOLDspi5_Pin});
}

MemoryErrorHandling::MEM_ERROR
STM32F429Wrapper::InitializeGPIO(GPIOPin GPIOPin, GPIO_PinState gpioPinState, uint32_t mode, uint32_t pull,
                                 uint32_t speed, uint32_t alternate)
{
    if (!GPIOPin.m_GPIOBank)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;


    GPIO_InitTypeDef gpioInitStruct = {0};
    gpioInitStruct.Pin = GPIOPin.m_GPIOPin;
    gpioInitStruct.Mode = mode;
    gpioInitStruct.Pull = pull;
    gpioInitStruct.Speed = speed;
    gpioInitStruct.Alternate = alternate;
    HAL_GPIO_Init(GPIOPin.m_GPIOBank, &gpioInitStruct);
    HAL_GPIO_WritePin(GPIOPin.m_GPIOBank, GPIOPin.m_GPIOPin, gpioPinState);
    m_InitializedGPIOPins.push_back(GPIOPin);

    return MemoryErrorHandling::MEM_NO_ERROR;
}


MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeGPIO(const GPIOPin &gpioPin)
{
    HAL_GPIO_DeInit(gpioPin.m_GPIOBank, gpioPin.m_GPIOPin);
    m_InitializedGPIOPins.remove(gpioPin);
    return MemoryErrorHandling::MEM_REGISTER_NOT_SET;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeGPIOBank(GPIO_TypeDef *GPIOBank)
{
    if (GPIOBank == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (GPIOBank == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (GPIOBank == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (GPIOBank == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (GPIOBank == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (GPIOBank == GPIOF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (GPIOBank == GPIOG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (GPIOBank == GPIOH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
    else if (GPIOBank == GPIOI)
        __HAL_RCC_GPIOI_CLK_ENABLE();
    else
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    m_InitializedGPIOBanks.push_back(GPIOBank);

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitializeGPIOBank(GPIO_TypeDef *GPIOBank)
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

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::InitializeInterface(std::string &interfaceName)
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

#endif // STM32
