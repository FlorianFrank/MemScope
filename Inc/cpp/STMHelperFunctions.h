/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef STM_MEASUREMENT_FIRMWARE_STMHELPERFUNCTIONS_H
#define STM_MEASUREMENT_FIRMWARE_STMHELPERFUNCTIONS_H

#include "MemoryErrorHandling.h"
#include <cstdint>

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

class STMHelperFunctions
{
public:
    static MEM_ERROR Delay(uint32_t milliseconds);
};


#endif //STM_MEASUREMENT_FIRMWARE_STMHELPERFUNCTIONS_H
