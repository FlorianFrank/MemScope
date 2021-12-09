/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include <cpp/MemoryControllerWrappers/MemoryControllerSPI.h>
#include "cpp/Main.h"
#include "cpp/ClockConfig.h"
#include "cpp/MemoryController.h"

extern "C" {
#include "SystemFiles/spi.h" // MX_SPI4_Init()
#include "SystemFiles/usart.h" // MX_UART4_Init()
#include "SystemFiles/gpio.h" // MX_GPIO_Init
#include "SystemFiles/fmc.h" // MX_FMC_Init()

#include "cpp/MemoryTest.h"
#if RDMON_SPECS
extern void initialise_monitor_handles(void);
#endif //RDMON_SPECS

}

bool running = true;


#define USE_USB_CDC // TODO

/**
  * @brief  The application entry point.
  * @retval int
  */
int main()
{
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

    //executeMemoryTest();

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

    SPIHandle handle = &hspi5;
    SPIWrapper uartWrapper(handle);
    MemoryControllerSPI memoryController(&uartWrapper);


    //TimeMeasurement timeMeasurement; // TODO resolve



    //MemoryTest memoryTest(&memoryController, &timeMeasurement);

    //memoryTest.executeMemoryTest();

    while (running)
    {
        if (uartWrapper.isTransferCplt())
        {
#ifdef USE_UART
            executeCommandUART(&huart4);
#endif


#ifdef USE_USB_CDC


#endif
            uartWrapper.resetTransferCplt();
        }

    }
    /* USER CODE END 3 */
}



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
