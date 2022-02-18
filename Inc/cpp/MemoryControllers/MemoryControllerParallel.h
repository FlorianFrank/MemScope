/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYCONTROLLERPARALLEL_H
#define MEMORY_TESTING_FW_MEMORYCONTROLLERPARALLEL_H

#include <cpp/Devices/DeviceWrapper.h>
#include "MemoryController.h"
#include "cpp/MemoryControllers/MemoryControllerParallelProperties.h"

/**
 * @brief This class is used to read and write to memory modules with a parallel connection.
 * Currently this class supports writing to memory modules using a STM32F429DISC1 board using memory mapped IO.
 */
class MemoryControllerParallel : public MemoryController
{
public:
    explicit MemoryControllerParallel(InterfaceWrapper *interfaceWrapper, MemoryModule &memoryModule,
                                      DeviceWrapper &deviceWrapper);

    MEM_ERROR Initialize() override;

    MEM_ERROR Write8BitWord(uint32_t adr, uint8_t value) override;

    MEM_ERROR Read8BitWord(uint32_t adr, uint8_t *retValue) override;

    MEM_ERROR Write16BitWord(uint32_t adr, uint16_t value) override;

    MEM_ERROR Read16BitWord(uint32_t adr, uint16_t *value) override;

private:
    MemoryControllerParallelProperties m_Properties;
};
#endif //MEMORY_TESTING_FW_MEMORYCONTROLLERPARALLEL_H
