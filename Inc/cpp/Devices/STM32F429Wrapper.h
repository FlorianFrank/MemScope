/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_STM32F429_WRAPPER_H
#define MEMORY_TESTING_FW_STM32F429_WRAPPER_H
#include "DeviceWrapper.h"
#include "cpp/MemoryErrorHandling.h"
#include "DeviceDefines.h"

#include <list>

#ifdef STM32
/**
 * @brief This class wraps functions used by the STM32F429 board.
 * Like initializing the system clock or the hardware abstraction layers.
 */
class STM32F429Wrapper: public DeviceWrapper
{
public:
    STM32F429Wrapper() = default;
    ~STM32F429Wrapper() override;

    MemoryErrorHandling::MEM_ERROR Initialize() override;
    MemoryErrorHandling::MEM_ERROR DeInitialize() override;
    MemoryErrorHandling::MEM_ERROR InitializeHardwareInterface(std::string &interfaceName) override;

private:

    MemoryErrorHandling::MEM_ERROR InitializeDefaultGPIOSettings();

    static MemoryErrorHandling::MEM_ERROR SystemClockConfig();

    MemoryErrorHandling::MEM_ERROR InitializeSPI4();
    MemoryErrorHandling::MEM_ERROR InitializeSPI5();

    __attribute__((unused)) MemoryErrorHandling::MEM_ERROR DeInitializeSPI4();

    __attribute__((unused)) MemoryErrorHandling::MEM_ERROR DeInitializeSPI5();

    MemoryErrorHandling::MEM_ERROR InitializeGPIOPin(GPIOPin gpioPin, GPIO_PinState gpioPinState, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t alternate);
    MemoryErrorHandling::MEM_ERROR DeInitializeGPIOPin(const GPIOPin &gpioPin);

    __attribute__((unused)) MemoryErrorHandling::MEM_ERROR InitializeGPIOBank(GPIOBank gpioBank);
    MemoryErrorHandling::MEM_ERROR DeInitializeGPIOBank(GPIOBank gpioBank);

    std::list<GPIOBank> m_InitializedGPIOBanks;
    std::list<GPIOPin> m_InitializedGPIOPins;
};
#endif // NOT_STM32
#endif // MEMORY_TESTING_FW_STM32F429_WRAPPER_H
