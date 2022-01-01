/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
#define MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an SRAM Cypress CY7C1051CDV33 memory chip.
 * See link <a href="https://www.mouser.de/datasheet/2/100/CYPR_S_A0011122244_1-2541178.pdf">Memory Datasheet</a>.
 */
class SRAM_Cypress_CY7C1051CDV33: public MemoryModule
{
public:
    SRAM_Cypress_CY7C1051CDV33();
    ~SRAM_Cypress_CY7C1051CDV33() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
