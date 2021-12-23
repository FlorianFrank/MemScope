//
// Created by florianfrank on 2021-12-23.
//

#include "cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h"

FRAM_Rohm_MR48V256CTAZAARL::FRAM_Rohm_MR48V256CTAZAARL(): MemoryModule(MemoryProperties::TYPE_FRAM,
                                                                       "FRAM_Rohm_MR48V256CTAZAARL",
                                                                       MemoryProperties::SPI, 16, 32768,
                                                                       {233, 358}, {3000, 3600})
{

}

MEM_ERROR FRAM_Rohm_MR48V256CTAZAARL::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}

FRAM_Rohm_MR48V256CTAZAARL::~FRAM_Rohm_MR48V256CTAZAARL() = default;