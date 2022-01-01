/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_SRAM_ISSI_IS61WV51216BLL_H
#define MEMORY_TESTING_FW_SRAM_ISSI_IS61WV51216BLL_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an SRAM ISSI IS61WV51216BLL memory chip.
 * See link \link https://www.mouser.de/datasheet/2/198/61-64WV51216-258283.pdf.
 */
class SRAM_ISSI_IS61WV51216BLL: public MemoryModule
{
public:
    SRAM_ISSI_IS61WV51216BLL();
    ~SRAM_ISSI_IS61WV51216BLL() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_SRAM_ISSI_IS61WV51216BLL_H
