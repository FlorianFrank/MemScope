/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#ifndef MEMORY_TESTING_FW_RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1_H
#define MEMORY_TESTING_FW_RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1_H
#include "MemoryModule.h"

class SPIWrapper;

/**
 * @brief This class sets the parameters for an an RERAM Fujitsu MB85AS4MTPF memory chip.
 * See link <a href="https://www.mouser.com/pdfDocs/MB85AS4MT.pdf">Memory Datasheet</a>.
 */
class ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1: public MemoryModule
{
public:
    ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1();
    ~ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1() override;

    MEM_ERROR Initialize() override;

private:
    SPIWrapper* m_SPIWrapper;
};
#endif //MEMORY_TESTING_FW_RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1_H
