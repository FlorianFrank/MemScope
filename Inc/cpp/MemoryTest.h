/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYTEST_H
#define MEMORY_TESTING_FW_MEMORYTEST_H

#include "MemoryErrorHandling.h"

class MemoryController;
class TimeMeasurement;

class MemoryTest
{
public:

    static MemoryErrorHandling::MEM_ERROR MemoryTestFactory(MemoryTest **memoryTest, MemoryController *memoryController, TimeMeasurement *timeMeasurement);

    virtual MemoryErrorHandling::MEM_ERROR init() = 0;
    virtual MemoryErrorHandling::MEM_ERROR running() = 0;
    virtual MemoryErrorHandling::MEM_ERROR done() = 0;


    int executeReadLatencyTestFRAMRohm();
    int executeMemoryTest();

protected:
    MemoryTest(MemoryController *memoryController, TimeMeasurement *timeMeasurement);
    MemoryController *m_MemoryController;
    TimeMeasurement *m_TimeMeasurement;
};


#endif //MEMORY_TESTING_FW_MEMORYTEST_H
