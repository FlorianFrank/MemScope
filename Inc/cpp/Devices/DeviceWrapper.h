/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_DEVICE_WRAPPER_H
#define MEMORY_TESTING_FW_DEVICE_WRAPPER_H
#include "cpp/MemoryErrorHandling.h"
#include "DeviceDefines.h"

#include <string> // std::string

/**
 * @brief This class serves as an abstract class providing mandatory interfaces for supported devices.
 * See class \see STM32F429Wrapper, here the functions Initialize(), DeInitialize() and InitializeHardwareInterface() must be defined.
 * These functions serve as wrapper for the device specific functions.
 */
class DeviceWrapper
{
public:
    DeviceWrapper();
    virtual ~DeviceWrapper()= default;

    /**
     * @brief In this method all mandatory initialization functions must be called. On STM32 devices, for example,
     * the hardware abstraction layer must be enabled and the system-clock must be initialized.
     * @return MEM_NO_ERROR if no error occurred.
     */
    virtual MemoryErrorHandling::MEM_ERROR Initialize();

    /**
     * @brief In this method all mandatory de-initialization functions must be called, like freeing occupied
     * resources or de-initializing the HAL layers.
     * @return MEM_NO_ERROR if no error occurred.
     */
    virtual MemoryErrorHandling::MEM_ERROR DeInitialize();

    /**
     * @brief This method is used to initialize hardware interfaces like a specific SPI port or an UART interface.
     *
     * @param interfaceName the interface name must be provided as string.
     * This allows an abstraction from device specific handles for certain interfaces.
     * @return MEM_NO_ERROR if no error occurred.
     */
    virtual MemoryErrorHandling::MEM_ERROR InitializeHardwareInterface(std::string& interfaceName);

    virtual MemoryErrorHandling::MEM_ERROR
    InitializeGPIOPin(GPIOPin pin, GPIOMode mode, GPIOState initialState, GPIOPin alternate);

    virtual MemoryErrorHandling::MEM_ERROR DeInitializeGPIOPin(const GPIOPin &gpioPin);
};
#endif //MEMORY_TESTING_FW_DEVICE_WRAPPER_H
