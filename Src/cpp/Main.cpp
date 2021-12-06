extern "C" {
#include "test_cases.h"
#include "SystemFiles/spi.h"
#include "SystemFiles/usart.h"
#include "SystemFiles/usb_device.h"
#include "SystemFiles/gpio.h"
#include "SystemFiles/fmc.h"
#include "error_handler.h"
#include "usb_handler.h"
}

#include <ctime>
#include <cpp/ClockConfig.h>

#include "cpp/MemoryController.h"
#include "cpp/Main.h"


#define USE_USB_CDC

#if RDMON_SPECS
extern "C" {
extern void initialise_monitor_handles();
};
#endif //RDMON_SPECS

/**
  * @brief  The application entry point.
  * @retval int
  */
int main()
{
    /* USER CODE BEGIN 1 */
#if RDMON_SPECS
    initialise_monitor_handles();
#endif // RDMON_SPECS

    HAL_Init();

    ClockConfig::SystemClock_Config();

    MX_GPIO_Init();
    MX_FMC_Init();
    MX_UART4_Init();
    MX_SPI4_Init();
    MX_SPI5_Init();

//    executeMemoryTest(); TODO

#ifdef USE_UART
    receive(&huart4, (uint8_t *)Rx_Data, 1); // activate uart rx interrupt every time receiving 1 byte
  //showHelp(&huart4); // UART
#endif

#ifdef USE_USB_CDC
    //detect whether USB is connected
    /*while(!Virtual_Com_Port_IsHostPortOpen())
    {
        HAL_Delay(100);
    }*/
    HAL_Delay(100);
    //showHelp(NULL);
#endif

    MemoryController memoryController;

    while (1)
    {

        if(memoryController.isTransferCplt())
        {
#ifdef USE_UART
            // executeCommandUART(&huart4);
#endif



#ifdef USE_USB_CDC


#endif
            memoryController.resetTransferCplt();
        }


        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
