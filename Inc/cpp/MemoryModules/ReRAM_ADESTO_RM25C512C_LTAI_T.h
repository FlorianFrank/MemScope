/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_RERAM_ADESTO_RM25C512C_LTAI_T_H
#define MEMORY_TESTING_FW_RERAM_ADESTO_RM25C512C_LTAI_T_H
#include "MemoryModule.h"

class SPIWrapper;

/**
 * @brief This class sets the parameters for an an RERAM ADESTO RM25C512C_LTAI_T memory chip.
 * See link <a href="https://www.mouser.de/datasheet/2/590/DS-RM25C512C_079-1385763.pdf">Memory Datasheet</a>.
 */
class RERAM_ADESTO_RM25C512C_LTAI_T: public MemoryModule
{
public:
    RERAM_ADESTO_RM25C512C_LTAI_T();
    ~RERAM_ADESTO_RM25C512C_LTAI_T() override;

    MEM_ERROR Initialize() override;

private:
    SPIWrapper *m_SPIWrapper;
};
#endif //MEMORY_TESTING_FW_RERAM_ADESTO_RM25C512C_LTAI_T_H
