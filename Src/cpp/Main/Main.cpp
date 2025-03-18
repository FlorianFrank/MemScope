/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/Main/Main.hpp"
#include "cpp/JSONParser.h"
#include <cpp/Devices/STM32F429Wrapper.h>
#include <cpp/InterfaceWrappers/UARTWrapper.h>
#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include <cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h>
#include "cpp/MemoryTest.h"

#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

// Global Flags and Data
bool isRunning = true;
bool isCallbackReceived = false;
std::string receivedCallbackData;

// Callback Function
void onCommandReceived(std::string &data) {
    receivedCallbackData = data;
    isCallbackReceived = true;
}

int main() {
#if STM32
    STM32F429Wrapper device;
    device.Initialize();
#endif

    FRAM_Rohm_MR48V256CTAZAARL fram;
    MemoryControllerParallel memoryController(nullptr, fram, device);
    memoryController.Initialize();

    auto uartWrapper = std::make_unique<UARTWrapper>(
            "USART1", 115200, UARTWrapper_TRANSMIT_RECEIVE,
            UARTWrapper_WORD_LENGTH_8, UARTWrapper_NO_PARITY,
            UARTWrapper_STOP_BITS_1, UART_MODE_INTERRUPT
    );
    uartWrapper->Initialize();
    uartWrapper->RegisterReceiveCallback(onCommandReceived);
    uartWrapper->ReceiveData(nullptr, nullptr, NON_BLOCKING, 0);

    PUFConfiguration config;
    MemoryTest test;

    // Main Loop
    while (isRunning) {
        if (isCallbackReceived) {
            parse_json(receivedCallbackData.c_str(), &config);
            isCallbackReceived = false;
            static MemoryErrorHandling::MEM_ERROR MemoryTestFactory(MemoryTest **memoryTest, MemoryController *memoryController, TimeMeasurement *timeMeasurement);

        }

        HAL_Delay(250);  // 250 ms delay
    }
}