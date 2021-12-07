//
// Created by florianfrank on 07.12.21.
//

#ifndef MEMORY_TESTING_FW_MEMORYCONTROLLERPARALLEL_H
#define MEMORY_TESTING_FW_MEMORYCONTROLLERPARALLEL_H

#include "cpp/MemoryController.h"


class MemoryControllerParallel : public MemoryController
{
public:
    explicit MemoryControllerParallel(InterfaceWrappers &interfaceWrapper);

    MEM_ERROR MemoryWrite8BitWord(uint32_t adr, uint8_t value) override;

    MEM_ERROR MemoryRead8BitWord(uint32_t adr, uint8_t *retValue) const override;

    MEM_ERROR MemoryWrite16BitWord(uint32_t adr, uint16_t value) override;

    MEM_ERROR MemoryRead16BitWord(uint32_t adr, uint16_t *value) const override;

};


#endif //MEMORY_TESTING_FW_MEMORYCONTROLLERPARALLEL_H
