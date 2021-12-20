/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/Devices/STM32F429Wrapper.h"

#if STM32
extern "C" {
#include "error_handler.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_pwr_ex.h"
#include <SystemFiles/gpio.h>


#if RDMON_SPECS
extern void initialise_monitor_handles();
#endif // RDMON_SPECS
}
#endif // STM32

/**
 * @brief This function initializes the clock configuration.
 * @return Error Code
 */

/*static*/ MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::SystemClock_Config()
{
#if STM32
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
#endif // STM32
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::Initialize()
{
#if STM32
#if RDMON_SPECS
    initialise_monitor_handles();
#endif // RDMON_SPECS

    HAL_StatusTypeDef ret = HAL_Init();
    if(ret == HAL_ERROR)
        return MemoryErrorHandling::MEM_IO_ERROR;
    if (ret == HAL_BUSY)
        return MemoryErrorHandling::MEM_HAL_BUSY;
    if (ret == HAL_TIMEOUT)
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;

    MX_GPIO_Init();

    return SystemClock_Config();
#else
    return MemoryErrorHandling::MEM_NO_ERROR;
#endif // STM32
}

MemoryErrorHandling::MEM_ERROR STM32F429Wrapper::DeInitialize()
{
#if STM32
    HAL_StatusTypeDef ret = HAL_DeInit();
    if(ret == HAL_ERROR)
        return MemoryErrorHandling::MEM_IO_ERROR;
    if (ret == HAL_BUSY)
        return MemoryErrorHandling::MEM_HAL_BUSY;
    if (ret == HAL_TIMEOUT)
        return MemoryErrorHandling::MEM_HAL_TIMEOUT;

    return MemoryErrorHandling::MEM_REGISTER_NOT_SET;
#else
    return MemoryErrorHandling::MEM_NO_ERROR;
#endif // STM32
}
