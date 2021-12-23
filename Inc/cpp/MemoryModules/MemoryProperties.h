//
// Created by florianfrank on 23.12.21.
//

#ifndef MEMORY_TESTING_FW_MEMORYPROPERTIES_H
#define MEMORY_TESTING_FW_MEMORYPROPERTIES_H

namespace MemoryProperties
{

    enum Type
    {
        TYPE_SRAM,
        TYPE_DRAM,
        TYPE_FLASH,
        TYPE_MRAM,
        TYPE_FRAM,
        TYPE_RERAM
    };

    enum Connection
    {
        PARALLEL, SPI
    };

    struct TemperatureProperties
    {
        double m_MinTemperature;
        double m_MaxTemperature;
    };

    struct VoltageProperties
    {
        double m_MinVoltage;
        double m_MaxVoltage;
    };
}

#endif //MEMORY_TESTING_FW_MEMORYPROPERTIES_H
