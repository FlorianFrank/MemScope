/**
 * @file Main.cpp
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 * @brief Main entry point for the STM32 memory testing application.
 */

#include "cpp/Main/Main.hpp"
#include "cpp/JSONParser.h"
#include <cpp/Devices/STM32F429Wrapper.h>
#include <cpp/InterfaceWrappers/UARTWrapper.h>
#include <cpp/MemoryControllers/MemoryControllerParallel.h>
#include <cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h>
#include "cpp/Experiments/MemoryExperiment.h"
#include "cpp/Experiments/RowHammering.h"
#include "Logger.h"
#include "general_defines.h"

#include <memory>
#include <string>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

bool isRunning = true;
bool isCallbackReceived = false;
bool continueFlagReceived = false;
std::string receivedCallbackData;

enum ExperimentState {
    NO_DEF,
    INIT,
    RUN,
    DONE
};

/**
 * @brief Callback function triggered on UART command reception.
 * @param data Received command data.
 */
void onCommandReceived(std::string &data, RecvMsgType receivedMsgType) {
    switch (receivedMsgType) {
        case MSG_CMD:
            receivedCallbackData = data;
            isCallbackReceived = true;
            break;
        case MSG_CONTINUE:
            continueFlagReceived = true;
            break;
        default:
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "Unknown message Type received -> ignore");
            break;
    }
}

/**
 * @brief Initializes the UART interface.
 * @param uartWrapper Unique pointer to UARTWrapper.
 * @return Memory error code.
 */
MEM_ERROR setupUARTInterface(std::unique_ptr<UARTWrapper> &uartWrapper) {
    uartWrapper = std::make_unique<UARTWrapper>(
            DEFAULT_UART_PORT, DEFAULT_BAUDRATE, UARTWrapper_TRANSMIT_RECEIVE,
            UARTWrapper_WORD_LENGTH_8, UARTWrapper_NO_PARITY,
            UARTWrapper_STOP_BITS_1, UART_MODE_INTERRUPT, 64
    );

    auto retCode = uartWrapper->Initialize();
    if (retCode != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART initialization failed: %s",
                    MemoryErrorHandling::memErrorToString(retCode));
        return retCode;
    }

    uartWrapper->RegisterReceiveCallback(onCommandReceived);
    retCode = uartWrapper->ReceiveData(nullptr, nullptr, NON_BLOCKING, 0);
    if (retCode != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "UART receive setup failed: %s",
                    MemoryErrorHandling::memErrorToString(retCode));
    }
    return retCode;
}

/**
 * @brief Main entry point.
 * @return Program exit code.
 */
int main() {
#if STM32
    STM32F429Wrapper device;
    device.Initialize();
#endif

    FRAM_Rohm_MR48V256CTAZAARL fram;
    std::unique_ptr<MemoryController> memoryController = std::make_unique<MemoryControllerParallel>(
            MemoryControllerParallel(nullptr, fram, device));

    std::unique_ptr<UARTWrapper> uartWrapper;
    auto ret = setupUARTInterface(uartWrapper);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
        Logger::log(LogLevel::INFO, __FILE__, __LINE__, "Exiting program due to UART setup failure.");
        return 0;
    }

    char *response = "R:idn:STM32_F429_DISC1\n";
    uint16_t strSize = strlen(response);
    uartWrapper->SendData(reinterpret_cast<uint8_t *>(response), &strSize, 1000, true);

    PUFConfiguration config;
    MemoryExperiment *experiment = nullptr;
    ExperimentState expState = NO_DEF;
    while (isRunning) {
        if (isCallbackReceived) {
            auto jsonRet = parse_json(receivedCallbackData.c_str(), &config);
            if (jsonRet == JSON_RECEIVED_CMD) {
                HAL_Delay(MAIN_LOOP_DELAY);
                isCallbackReceived = false;
                expState = INIT;
                MemoryExperiment::MemoryTestFactory(&experiment, *memoryController, config, *uartWrapper);
                experiment->init();
                if (config.generalConfig.pufType != VOLTAGE_VARIATIONS) {
                    experiment->running();
                    experiment->done();
                }
            }
        }

        if (experiment) {
            if (continueFlagReceived) {
                continueFlagReceived = false;
                if (config.generalConfig.pufType == VOLTAGE_VARIATIONS) {
                    if (expState == INIT) {
                        experiment->running();
                        expState = RUN;
                    } else if (expState == RUN) {
                        experiment->done();
                        expState = NO_DEF;
                    }
                    experiment->notifyContinue();
                }
            }
        }
        HAL_Delay(MAIN_LOOP_DELAY);
    }
}
