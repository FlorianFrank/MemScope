//
// Created by florianfrank on 2021-12-23.
//

#include "cpp/MemoryModules/ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"

MEM_ERROR ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::Initialize()
{
    return MemoryModule::Initialize(m_SPIWrapper);
}

ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::~ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1()
{
    delete m_SPIWrapper;
}

ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1(): MemoryModule(MemoryProperties::TYPE_RERAM,
                                                                                       "ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1",
                                                                                       MemoryProperties::SPI, 8, 524288, {233, 358},
                                                                                       {3000, 3600})
{
    m_SPIWrapper = new SPIWrapper("SPI5");
}
