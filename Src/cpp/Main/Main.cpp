/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */

#include <cpp/Devices/STM32F429Wrapper.h>
#include <cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h>
#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include "cpp/Main/Main.hpp"



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
    FRAM_Rohm_MR48V256CTAZAARL fram;

    MemoryControllerParallel memoryController(nullptr, fram);
    memoryController.Initialize();

    printf("START\n");
    for (uint32_t i = 0; i < fram.GetMemorySize(); i++)
    {
        memoryController.Write8BitWord(i, 0xFF);
        HAL_Delay(1);

    }

    printf("START READING\n");

    uint8_t retValue = 0x00;
    for (uint32_t i = 0; i < fram.GetMemorySize(); i++)
    {
        memoryController.Read8BitWord(i, &retValue);
    }

    int ctr = 0;
    if(retValue != 0xFF)
    {
        ctr++;
    }

    printf("CTR %d\n", ctr);
}
