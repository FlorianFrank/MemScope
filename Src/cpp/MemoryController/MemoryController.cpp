/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include "cpp/MemoryControllers/MemoryController.h"

#include "io_pin_defines.h"
#include "Logger.h"

extern "C" {
#include "metrics.h"
}

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;


MemoryController::MemoryController(DeviceWrapper &deviceWrapper, InterfaceWrapper *interfaceWrapper,
                                   MemoryModule &memoryModule)
        : write_mode(0xFF), len(0), STRING_BUFFER(""),
          m_MMIOStartAddress(MEMORY_BANK_ADDRESS),
          srambp(nullptr), total_one(0), total_zero(0),
          flipped_one(0), flipped_zero(0), m_MemoryModule(memoryModule), m_DeviceWrapper(deviceWrapper) {
    m_InterfaceWrapper = interfaceWrapper;
}

MemoryController::~MemoryController() = default;

MEM_ERROR MemoryController::FillMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t value) {
    for (uint32_t adr = startAddress; adr < endAddress; adr++) {
        MEM_ERROR ret;
        auto bitWidth = m_MemoryModule.GetBitWidth();
        if (bitWidth == 8)
            ret = Write8BitWord(adr, value);
        else if (bitWidth == 16)
            ret = Write16BitWord(adr, value);
        else {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Bit width %d not supported", bitWidth);
            ret = MEM_ERROR::MEM_BIT_LEN_NOT_SUPPORTED;
        }

        if (ret != MemoryErrorHandling::MEM_NO_ERROR){
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__,  "Error while performing write operation %s", MemoryErrorHandling::memErrorToString(ret));
            return ret;
        }

    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR
MemoryController::FillMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t(*dataFunction)(uint32_t)) {
    for (uint32_t adr = startAddress; adr < endAddress; adr++) {
        auto bitWidth = m_MemoryModule.GetBitWidth();
        MEM_ERROR ret;
        if (bitWidth == 8)
            ret = Write8BitWord(adr, dataFunction(adr));
        else if (bitWidth == 16)
            ret = Write16BitWord(adr, dataFunction(adr));
        else {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Bit width %d not supported", bitWidth);
            ret = MEM_ERROR::MEM_BIT_LEN_NOT_SUPPORTED;
        }


        if (ret != MemoryErrorHandling::MEM_NO_ERROR) {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Error while performing write operation %s",
                        MemoryErrorHandling::memErrorToString(ret));
            return ret;
        }

    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryController::VerifyMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t expectedValue) {
    for (uint32_t adr = startAddress; adr < endAddress; adr++) {
        uint16_t trueValue;

        MEM_ERROR ret;
        auto bitWidth = m_MemoryModule.GetBitWidth();
        if (bitWidth == 8) {
            uint8_t readValue = 0;
            ret = Read8BitWord(adr, &readValue);
            trueValue = readValue;
        } else if (bitWidth == 16) {
            uint16_t readValue = 0;
            ret = Read16BitWord(adr, &readValue);
            trueValue = readValue;
        }else {
                Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Bit width %d not supported", bitWidth);
                ret = MEM_ERROR::MEM_BIT_LEN_NOT_SUPPORTED;
            }
        if (ret != MemoryErrorHandling::MEM_NO_ERROR){
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Error while performing write operation %s",
                        MemoryErrorHandling::memErrorToString(ret));
            return ret;
        }
        if (trueValue != expectedValue){
            Logger::log(LogLevel::WARNING, __FILE__, __LINE__, "Read value != expected value (0x%x != 0x%x)", trueValue, expectedValue);
            return MemoryErrorHandling::MEM_NOT_WRITTEN;
        }
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}


MEM_ERROR
MemoryController::VerifyMemoryArea(uint32_t startAddress, uint32_t endAddress, uint16_t(*dataFunction)(uint32_t)) {
    for (uint32_t adr = startAddress; adr < endAddress; adr++) {
        uint16_t trueValue;

        MEM_ERROR ret;
        auto bitWidth = m_MemoryModule.GetBitWidth();
        if (bitWidth == 8) {
            uint8_t readValue = 0;
            ret = Read8BitWord(adr, &readValue);
            trueValue = readValue;
        } else if (bitWidth == 16) {
            uint16_t readValue = 0;
            ret = Read16BitWord(adr, &readValue);
            trueValue = readValue;
        } else {
            Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Bit width %d not supported", bitWidth);
            ret = MEM_ERROR::MEM_BIT_LEN_NOT_SUPPORTED;
        }

        if (ret != MemoryErrorHandling::MEM_NO_ERROR)
            return ret;
        if (trueValue != dataFunction(adr))
            return MemoryErrorHandling::MEM_NOT_WRITTEN;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}


/*
 * @brief								fills the whole SRAM with 0's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillWithZeros() {
    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

    if (m_MemoryModule.GetBitWidth() == 16)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint16_t>(0x0000));
    else if (m_MemoryModule.GetBitWidth() == 8)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint8_t>(0x00));

    Logger::log(LogLevel::ERROR, __FILE__, __LINE__, "Bit width %d not supported", m_MemoryModule.GetBitWidth());
    return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
}


MEM_ERROR MemoryController::FillWithZerosAndVerifyRead() {
    MEM_ERROR ret = MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
    if (m_MemoryModule.GetBitWidth() == 16)
        ret = FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint16_t>(0x0000));
    else if (m_MemoryModule.GetBitWidth() == 8)
        ret = FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint8_t>(0x00));

    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    if (m_MemoryModule.GetBitWidth() == 16)
        return VerifyMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint16_t>(0x00));
    else if (m_MemoryModule.GetBitWidth() == 8)
        return VerifyMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint8_t>(0x000));

    return ret;
}


/*
 * @brief								fills the whole SRAM with 1's
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillWithOnes() {
    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

    if (m_MemoryModule.GetBitWidth() == 16)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint16_t>(0xFFFF));
    else if (m_MemoryModule.GetBitWidth() == 8)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint8_t>(0xFF));

    return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
}

/*
 * @brief								fills the first address SRAM with the argument and increment it by one
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 * @param uint32_t *m_arguments			the start value to count up
 */
MEM_ERROR MemoryController::FillMemoryWithAscendingValues() {
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    MEM_ERROR ret = MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
    if (m_MemoryModule.GetBitWidth() == 16)
        ret = FillMemoryArea(0, m_MemoryModule.GetMemorySize(), [](uint32_t address) {
            return static_cast<uint16_t>(address % 0xFFFF);
        });
    else if (m_MemoryModule.GetBitWidth() == 8)
        ret = FillMemoryArea(0, m_MemoryModule.GetMemorySize(), [](uint32_t address) {
            return static_cast<uint16_t>(address % 0xFFFF);
        });

    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR
MemoryController::FillMemoryWithAscendingValuesAndVerifyRead(uint32_t *buffLen, const uint32_t *arguments) {
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

#if MEM_ACCESS_WIDTH_BIT == 16


#elif MEM_ACCESS_WIDTH_BIT == 8
    auto countFunction = [](uint32_t address)
    {
        return static_cast<uint8_t>(address % 0xFF);
    };
#endif // MEM_ACCESS_WIDTH_BIT == 16
    MEM_ERROR ret = MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
    ret = FillMemoryArea(0, m_MemoryModule.GetMemorySize(), [](uint32_t address) {
        return static_cast<uint16_t>(address % 0xFFFF);
    });

    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    if (m_MemoryModule.GetBitWidth() == 16)
        return VerifyMemoryArea(0, m_MemoryModule.GetMemorySize(), [](uint32_t address) {
            return static_cast<uint16_t>(address % 0xFFFF);
        });

    return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
}

/*
 * @brief								fills the whole SRAM with alternating 010101010....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillMemoryWithAlternatingZeroAndOne() {
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

    if (m_MemoryModule.GetBitWidth() == 16)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint16_t>(0x5555));
    else if (m_MemoryModule.GetBitWidth() == 8)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint8_t>(0x55));

    return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
}

/*
 * @brief								fills the whole SRAM with alternating 101010101....
 * @param UART_HandleTypeDef huart*		the UART handler to communicate with the user
 */
MEM_ERROR MemoryController::FillMemoryWithAlternatingOneAndZero() {
    // reset the counter for statistical analysis
    InitCounter();
    // reset the m_arguments
    InitArguments();

    // reset the counter for statistical analysis // TODO was hat es mit den counter auf sich?
    InitCounter();
    // reset the m_arguments
    InitArguments();

    if (m_MemoryModule.GetBitWidth() == 16)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint16_t>(0xAAAA));
    else if (m_MemoryModule.GetBitWidth() == 8)
        return FillMemoryArea(0, m_MemoryModule.GetMemorySize(), static_cast<uint8_t>(0xAA));

    return MemoryErrorHandling::MEM_BIT_LEN_NOT_SUPPORTED;
}


/*
 * @brief							resets the counters for statistical analysis
 */
void MemoryController::InitCounter() {
    total_one = 0;
    total_zero = 0;
    flipped_one = 0;
    flipped_zero = 0;
}

/*
 * @brief							resets the custom start/end addresses and the custom start value
 * 									these variables are set in a write method
 */
void MemoryController::InitArguments() {
    start_adr = 0;
    end_adr = 0;
    start_value = 0;
}

/*static*/ bool MemoryController::IsInvalidAddress(uint32_t address, uint32_t size) {
    return (address >= size);
}

MEM_ERROR MemoryController::Initialize() {
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Write8BitWord(uint32_t adr, uint8_t value) {
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Read8BitWord(uint32_t adr, uint8_t *ret) {
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Write16BitWord(uint32_t adr, uint16_t value) {
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::Read16BitWord(uint32_t adr, uint16_t *value) {
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}

MEM_ERROR MemoryController::SetTimingParameters(PUFConfiguration &pufConfig){
    return MemoryErrorHandling::MEM_MEMORY_CONTROLLER_NOT_SPECIFIED;
}
