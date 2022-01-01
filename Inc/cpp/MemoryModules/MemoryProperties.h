/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYPROPERTIES_H
#define MEMORY_TESTING_FW_MEMORYPROPERTIES_H

namespace MemoryProperties
{
    /**
     * @brief This enum defines all possible tyes of memories supported by this library.
     */
    enum Type
    {
        TYPE_SRAM,
        TYPE_DRAM,
        TYPE_FLASH,
        TYPE_MRAM,
        TYPE_FRAM,
        TYPE_RERAM
    };

    /**
     * @brief This enum defines the suppported connection type like a parallel connection or over a SPI connection.
     */
    enum Connection
    {
        PARALLEL,
        SPI
    };

    /**
     * @brief This struct defines the min and max temperatures supported by a specific memory type.
     */
    struct TemperatureProperties
    {
        double m_MinTemperature;
        double m_MaxTemperature;
    };

    /**
     * @brief This struct defines the min and max supply voltages supported by a specific memory type.
     */
    struct VoltageProperties
    {
        double m_MinVoltage;
        double m_MaxVoltage;
    };
}
#endif //MEMORY_TESTING_FW_MEMORYPROPERTIES_H
