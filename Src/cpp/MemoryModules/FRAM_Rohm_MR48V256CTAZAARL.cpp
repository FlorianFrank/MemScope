/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/FRAM_Rohm_MR48V256CTAZAARL.h"

FRAM_Rohm_MR48V256CTAZAARL::FRAM_Rohm_MR48V256CTAZAARL(): MemoryModule(MemoryProperties::TYPE_FRAM,
                                                                       "FRAM_Rohm_MR48V256CTAZAARL",
                                                                       MemoryProperties::SPI, 16, 32768,
                                                                       {233, 358}, {3000, 3600})
{}

FRAM_Rohm_MR48V256CTAZAARL::~FRAM_Rohm_MR48V256CTAZAARL() = default;

MEM_ERROR FRAM_Rohm_MR48V256CTAZAARL::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}

