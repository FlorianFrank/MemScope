/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYTEST_H
#define MEMORY_TESTING_FW_MEMORYTEST_H

#include "cpp/MemoryErrorHandling.h"
#include "cpp/JSONParser.h"
#include "cpp/MemoryControllers/MemoryController.h"

#include <vector>


class MemoryController;

class TimeMeasurement;

class MemoryExperiment {
public:

    static MemoryErrorHandling::MEM_ERROR
    MemoryTestFactory(MemoryExperiment **experiment, MemoryController &memoryController,
                      PUFConfiguration &pufConfiguration);
    static MemoryErrorHandling::MEM_ERROR deleteExperiments();

    MemoryErrorHandling::MEM_ERROR  configureMemoryController();

    virtual MemoryErrorHandling::MEM_ERROR init() = 0;
    virtual MemoryErrorHandling::MEM_ERROR running() = 0;
    virtual MemoryErrorHandling::MEM_ERROR done() = 0;

    virtual ~MemoryExperiment() = default;

protected:
    MemoryExperiment(MemoryController &memoryController, PUFConfiguration &pufConfig);


    MemoryErrorHandling::MEM_ERROR  initializeMemory();

    MemoryController m_MemoryController;
    PUFConfiguration m_PUFConfiguration;

private:
    static std::vector<MemoryExperiment*> experiments;
};


#endif //MEMORY_TESTING_FW_MEMORYTEST_H
