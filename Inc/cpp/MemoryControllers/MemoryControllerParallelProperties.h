//
// Created by florianfrank on 18.02.22.
//

#ifndef MEMORYCONTROLLER_ELF_MEMORYCONTROLLERPARALLELPROPERTIES_H
#define MEMORYCONTROLLER_ELF_MEMORYCONTROLLERPARALLELPROPERTIES_H

#include "cpp/Devices/DeviceDefines.h"

#include <vector>

#if STM32F429xx

class MemoryControllerParallelProperties {
private:

    std::vector<GPIOPin> m_AddressLines = {
#if STM32F429xx
            {IO_BANK_F, IO_PIN_0,  "Addr 0"},
            {IO_BANK_F, IO_PIN_1,  "Addr 1"},
            {IO_BANK_F, IO_PIN_2,  "Addr 2"},
            {IO_BANK_F, IO_PIN_3,  "Addr 3"},
            {IO_BANK_F, IO_PIN_4,  "Addr 4"},
            {IO_BANK_F, IO_PIN_5,  "Addr 5"},
            {IO_BANK_F, IO_PIN_12, "Addr 6"},
            {IO_BANK_F, IO_PIN_13, "Addr 7"},
            {IO_BANK_F, IO_PIN_14, "Addr 8"},
            {IO_BANK_F, IO_PIN_15, "Addr 9"},
            {IO_BANK_G, IO_PIN_0,  "Addr 10"},
            {IO_BANK_G, IO_PIN_1,  "Addr 11"},
            {IO_BANK_G, IO_PIN_2,  "Addr 12"},
            {IO_BANK_G, IO_PIN_3,  "Addr 13"},
            {IO_BANK_G, IO_PIN_4,  "Addr 14"},
            {IO_BANK_G, IO_PIN_5,  "Addr 15"},
            {IO_BANK_D, IO_PIN_11, "Addr 16"},
            {IO_BANK_D, IO_PIN_12, "Addr 17"},
            {IO_BANK_D, IO_PIN_13, "Addr 18"},
            {IO_BANK_E, IO_PIN_3,  "Addr 19"},
            {IO_BANK_E, IO_PIN_4,  "Addr 20"},
#endif // STM32F429xx
    };

    std::vector<GPIOPin> m_DataLines {
#if STM32F429xx
            {IO_BANK_D, IO_PIN_14,  "I/0 0"},
            {IO_BANK_D, IO_PIN_15,  "I/0 1"},
            {IO_BANK_D, IO_PIN_0,   "I/0 2"},
            {IO_BANK_D, IO_PIN_1,   "I/0 3"},
            {IO_BANK_E, IO_PIN_7,   "I/0 4"},
            {IO_BANK_E, IO_PIN_8,   "I/0 5"},
            {IO_BANK_E, IO_PIN_9,   "I/0 6"},
            {IO_BANK_E, IO_PIN_10,  "I/0 7"},
            {IO_BANK_E, IO_PIN_11,  "I/0 8"},
            {IO_BANK_E, IO_PIN_12,  "I/0 9"},
            {IO_BANK_E, IO_PIN_13,  "I/0 10"},
            {IO_BANK_E, IO_PIN_14,  "I/0 11"},
            {IO_BANK_E, IO_PIN_15,  "I/0 12"},
            {IO_BANK_D, IO_PIN_8,   "I/0 13"},
            {IO_BANK_D, IO_PIN_9,   "I/0 14"},
            {IO_BANK_D, IO_PIN_10,  "I/0 15"},
#endif // STM32F429xx
    };

#if STM32F429xx
    GPIOPin m_WE = {IO_BANK_D, IO_PIN_5, "WE"};
    GPIOPin m_OE {IO_BANK_D, IO_PIN_4, "OE"};
    GPIOPin m_CS = {IO_BANK_G, IO_PIN_10, "CS"};
    GPIOPin m_LB = {IO_BANK_E, IO_PIN_0,  "LB"};
    GPIOPin m_UB = {IO_BANK_E, IO_PIN_1,  "UB"};
#else
    GPIOPin m_WE;
    GPIOPin m_OE;
    GPIOPin m_CS;
    GPIOPin m_LB;
    GPIOPin m_UB;
#endif // STM32F429xx

public:
    MemoryControllerParallelProperties() = default;

    GPIOPin GetAddress(const uint8_t address) const {
        if(address > m_AddressLines.size() -1 )
            return {IO_PIN_UNDEFINED, IO_PIN_UNDEFINED, "Address out of range"};
        return m_AddressLines.at(address);
    }

    GPIOPin GetDataLine(const uint8_t dataLine) const{
        if(dataLine > m_DataLines.size() -1 )
            return {IO_PIN_UNDEFINED, IO_PIN_UNDEFINED, "Dataline out of range"};
        return m_DataLines.at(dataLine);
    }

    GPIOPin& GetCS() { return m_CS; }
    GPIOPin& GetWE() { return m_WE; }
    GPIOPin& GetOE() { return m_OE; }
    GPIOPin& GetLB() { return m_LB; }
    GPIOPin& GetUB() { return m_UB; }

    auto& GetAddressLinesList() const {
        return m_AddressLines;
    }

    auto& GetDataLinesList() const {
        return m_DataLines;
    }
};

#endif // STM32F429xx

#endif //MEMORYCONTROLLER_ELF_MEMORYCONTROLLERPARALLELPROPERTIES_H
