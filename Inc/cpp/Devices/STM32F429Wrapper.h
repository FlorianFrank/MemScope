/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_STM32F429WRAPPER_H
#define MEMORY_TESTING_FW_STM32F429WRAPPER_H
#ifdef STM32
#include "cpp/MemoryErrorHandling.h"
#include "DeviceWrapper.h"
#include "DeviceDefines.h"

#include <stm32f429xx.h>
#include <stm32f4xx_hal_gpio.h>

#include <vector>
#include <list>


/**
 * @brief This class wraps functions used by the STM32F429 board.
 * Like initializing the system clock or the hardware abstraction layers.
 */
class STM32F429Wrapper: public DeviceWrapper
{
public:
    STM32F429Wrapper();

    ~STM32F429Wrapper() override;

    MemoryErrorHandling::MEM_ERROR Initialize() override;
    MemoryErrorHandling::MEM_ERROR DeInitialize() override;

    MemoryErrorHandling::MEM_ERROR InitializeHardwareInterface(std::string &interfaceName) override;

private:
    static MemoryErrorHandling::MEM_ERROR SystemClock_Config();

    MemoryErrorHandling::MEM_ERROR InitializeSPI4();
    MemoryErrorHandling::MEM_ERROR InitializeSPI5();

    MemoryErrorHandling::MEM_ERROR DeInitializeSPI4();
    MemoryErrorHandling::MEM_ERROR DeInitializeSPI5();

    MemoryErrorHandling::MEM_ERROR
    InitializeGPIO(GPIOPin GPIOPin, GPIO_PinState gpioPinState, uint32_t mode, uint32_t pull, uint32_t speed,
                   uint32_t alternate);
    MemoryErrorHandling::MEM_ERROR DeInitializeGPIO(const GPIOPin &gpioPin);

    MemoryErrorHandling::MEM_ERROR InitializeGPIOBank(GPIO_TypeDef *GPIOBank);
    MemoryErrorHandling::MEM_ERROR DeInitializeGPIOBank(GPIO_TypeDef *GPIOBank);

    std::vector<GPIO_TypeDef*> m_InitializedGPIOBanks;
    std::list<GPIOPin> m_InitializedGPIOPins;

};

#endif // NOT_STM32
#endif //MEMORY_TESTING_FW_STM32F429WRAPPER_H
