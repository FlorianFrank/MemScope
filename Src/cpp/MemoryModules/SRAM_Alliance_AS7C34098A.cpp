/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/SRAM_Alliance_AS7C34098A.h"


SRAM_Alliance_AS7C34098A::SRAM_Alliance_AS7C34098A(): MemoryModule(MemoryProperties::TYPE_SRAM,
                                                                   "SRAM_Alliance_AS7C34098A",
                                                                   MemoryProperties::PARALLEL, 16, 262144,
                                                                   {233, 358}, {3000, 3600})
{}

SRAM_Alliance_AS7C34098A::~SRAM_Alliance_AS7C34098A() = default;

MEM_ERROR SRAM_Alliance_AS7C34098A::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}
