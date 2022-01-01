/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include "cpp/MemoryModules/SRAM_Alliance_AS7C38098A.h"


SRAM_Alliance_AS7C38098A::SRAM_Alliance_AS7C38098A(): MemoryModule(MemoryProperties::TYPE_SRAM,
                                                                   "SRAM_Alliance_AS7C38098A",
                                                                   MemoryProperties::PARALLEL, 16, 524288,
                                                                   {233, 358}, {3000, 3600})
{}

SRAM_Alliance_AS7C38098A::~SRAM_Alliance_AS7C38098A() = default;

MEM_ERROR SRAM_Alliance_AS7C38098A::Initialize()
{
    return MemoryModule::Initialize(nullptr);
}
