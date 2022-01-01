/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C38098A_H
#define MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C38098A_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an SRAM Alliance AS7C38098A memory chip.
 * See link <a href="https://www.mouser.de/datasheet/2/12/AS7C38098A-1288225.pdf">Memory Datasheet</a>.
 */
class SRAM_Alliance_AS7C38098A: public MemoryModule
{
    SRAM_Alliance_AS7C38098A();
    ~SRAM_Alliance_AS7C38098A() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C38098A_H
