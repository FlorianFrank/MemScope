/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/ReRAM_ADESTO_RM25C512C_LTAI_T.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"


RERAM_ADESTO_RM25C512C_LTAI_T::RERAM_ADESTO_RM25C512C_LTAI_T() : MemoryModule(MemoryProperties::TYPE_RERAM,
                                                                              "RERAM_ADESTO_RM25C512C_LTAI_T",
                                                                              MemoryProperties::SPI, 8, 512,
                                                                              {233, 358}, {3000, 3600})
{
    m_SPIWrapper = new SPIWrapper("SPI5"); // TODO
}

RERAM_ADESTO_RM25C512C_LTAI_T::~RERAM_ADESTO_RM25C512C_LTAI_T()
{
    delete m_SPIWrapper;
}

MEM_ERROR RERAM_ADESTO_RM25C512C_LTAI_T::Initialize()
{
    return MemoryModule::Initialize(m_SPIWrapper);
}
