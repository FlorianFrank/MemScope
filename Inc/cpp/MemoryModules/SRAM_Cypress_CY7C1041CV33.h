/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
#define MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1041CV33_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an SRAM Cypress CY7C1041CV33 memory chip.
 * See link <a href="https://www.cypress.com/file/38796/download">Memory Datasheet</a>.
 */
class SRAM_Cypress_CY7C1041CV33 : public MemoryModule
{
public:
    SRAM_Cypress_CY7C1041CV33();
    ~SRAM_Cypress_CY7C1041CV33() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_SRAM_CYPRESS_CY7C1051CDV33_H
