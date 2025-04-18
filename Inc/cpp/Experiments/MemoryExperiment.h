/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYTEST_H
#define MEMORY_TESTING_FW_MEMORYTEST_H

#include "cpp/MemoryErrorHandling.h"
#include "cpp/JSONParser.h"
#include "cpp/MemoryControllers/MemoryController.h"
#include "cpp/InterfaceWrappers/InterfaceWrapper.h"

#include <vector>
#include <string.h> // memcpy



class MemoryController;

class TimeMeasurement;

class MemoryExperiment {
public:

    static MemoryErrorHandling::MEM_ERROR
    MemoryTestFactory(MemoryExperiment **experiment, MemoryController &memoryController,
                      PUFConfiguration &pufConfiguration, InterfaceWrapper &interfaceWrapper);
    static MemoryErrorHandling::MEM_ERROR deleteExperiments();

    MemoryErrorHandling::MEM_ERROR  configureMemoryController();

    virtual MemoryErrorHandling::MEM_ERROR init() = 0;

    virtual MemoryErrorHandling::MEM_ERROR running() = 0;

    virtual MemoryErrorHandling::MEM_ERROR done() = 0;

    virtual ~MemoryExperiment() = default;

    inline bool getContinueState() const { return m_Continue; };
    inline void notifyContinue() { m_Continue = true; }

protected:
    MemoryExperiment(MemoryController &memoryController, PUFConfiguration &pufConfig,
                     InterfaceWrapper &interfaceWrapper);


    MemoryErrorHandling::MEM_ERROR initializeMemory(uint16_t initValue);
    void sendProcessingStarted();
    void sendMessageFinished();
    MemoryErrorHandling::MEM_ERROR sendCurrentState(const char* state);

    MemoryErrorHandling::MEM_ERROR waitContinue() const;
    inline void resetContinue() { m_Continue = false; }

    MemoryController& m_MemoryController;
    PUFConfiguration m_PUFConfiguration;
    InterfaceWrapper& m_InterfaceWrapper;
private:
    bool m_Continue = false;
    static std::vector<MemoryExperiment *> experiments;

};


#endif //MEMORY_TESTING_FW_MEMORYTEST_H
