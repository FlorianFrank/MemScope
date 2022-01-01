/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_FRAM_FUJITSU_MB85R1001ANC_GE1_H
#define MEMORY_TESTING_FW_FRAM_FUJITSU_MB85R1001ANC_GE1_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an FRAM Fujitsu MB85R1001ANC GE1 memory chip.
 * See link <a href="https://www.fujitsu.com/downloads/MICRO/fme/fram/datasheet-MB85R1001A.pdf">Memory Datasheet</a>.
 */
class FRAM_Fujitsu_MB85R1001ANC_GE1: public MemoryModule
{
public:
    FRAM_Fujitsu_MB85R1001ANC_GE1();
    ~FRAM_Fujitsu_MB85R1001ANC_GE1() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_FRAM_FUJITSU_MB85R1001ANC_GE1_H
