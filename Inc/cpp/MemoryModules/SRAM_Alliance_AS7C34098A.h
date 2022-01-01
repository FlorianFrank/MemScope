/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C34098A_H
#define MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C34098A_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an SRAM Alliance AS7C34098A memory chip.
 * See link \link https://www.mouser.de/datasheet/2/12/as7c34098a_v2.2-1288185.pdf.
 */
class SRAM_Alliance_AS7C34098A: public MemoryModule
{
protected:
    SRAM_Alliance_AS7C34098A();
    ~SRAM_Alliance_AS7C34098A() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_SRAM_ALLIANCE_AS7C34098A_H
