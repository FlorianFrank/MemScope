/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include <cpp/MemoryControllers/MemoryControllerSPI.h>
#include <cstdio>
#include <cpp/InterfaceWrappers/UARTWrapper.h>
#include <cstring>
#include <cpp/Devices/STM32F429Wrapper.h>
#include <cpp/MemoryModules/ReRAM_ADESTO_RM25C512C_LTAI_T.h>
#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include "cpp/Main/Main.h"
#include "cpp/MemoryControllers/MemoryController.h"

#include "cpp/MemoryTest.h"
#include "cpp/Devices/STM32F429Wrapper.h"

bool running = true;


/**
  * @brief  The application entry point.
  * @retval int
  */
int main()
{

#if STM32
    STM32F429Wrapper device;
    device.Initialize();
#endif
    RERAM_ADESTO_RM25C512C_LTAI_T fram;

    MemoryControllerParallel memoryController(nullptr, fram);
    memoryController.Initialize();

/*    for (uint32_t i = 0; i < fram.GetMemorySize(); i++)
    {
        memoryController.Write8BitWord(i, 0xFF);
        HAL_Delay(10);

    }*/

    uint8_t retValue = 0x00;
    for (uint32_t i = 0; i < fram.GetMemorySize(); i++)
    {
        memoryController.Read8BitWord(i, &retValue);

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
