/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_FRAM_CYPRESS_FM22L16_55_TG_H
#define MEMORY_TESTING_FW_FRAM_CYPRESS_FM22L16_55_TG_H
#include "MemoryModule.h"

/**
 * @brief This class sets the parameters for an an FRAM Cypress FM22L16 55 TG memory chip.
 * See link \link https://www.mouser.de/datasheet/2/100/CYPR_S_A0011121120_1-2541176.pdf.
 */
class FRAM_Cypress_FM22L16_55_TG: public MemoryModule
{
public:
    FRAM_Cypress_FM22L16_55_TG();
    ~FRAM_Cypress_FM22L16_55_TG() override;

    MEM_ERROR Initialize() override;
};
#endif //MEMORY_TESTING_FW_FRAM_CYPRESS_FM22L16_55_TG_H
