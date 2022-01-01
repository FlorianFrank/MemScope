/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_FRAM_ROHM_MR48V256CTAZAARL_H
#define MEMORY_TESTING_FW_FRAM_ROHM_MR48V256CTAZAARL_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an FRAM Rohm MR48V256CTAZAARL memory chip.
 * See link <a href="https://www.mouser.de/datasheet/2/348/ROHM-06-04-2018-FEDR48V256C-01-1500402.pdf">Memory Datasheet</a>.
 */
class FRAM_Rohm_MR48V256CTAZAARL: public MemoryModule
{
public:
    FRAM_Rohm_MR48V256CTAZAARL();
    ~FRAM_Rohm_MR48V256CTAZAARL() override;

    MEM_ERROR Initialize() override;

};
#endif //MEMORY_TESTING_FW_FRAM_ROHM_MR48V256CTAZAARL_H
