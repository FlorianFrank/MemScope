/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_MRAM_EVERSPIN_MR4A08BUYS45_H
#define MEMORY_TESTING_FW_MRAM_EVERSPIN_MR4A08BUYS45_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an MRAM Everspin MR4A08BUYS45 memory chip.
 * See link <a href="https://www.mouser.de/datasheet/2/144/MR4A08B_Datasheet-1511665.pdf">Memory Datasheet</a>.
 */
class MRAM_Everspin_MR4A08BUYS45: public MemoryModule
{
public:
    MRAM_Everspin_MR4A08BUYS45();
    ~MRAM_Everspin_MR4A08BUYS45() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_MRAM_EVERSPIN_MR4A08BUYS45_H
