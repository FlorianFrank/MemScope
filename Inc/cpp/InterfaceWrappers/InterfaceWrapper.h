/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_INTERFACEWRAPPER_H
#define MEMORY_TESTING_FW_INTERFACEWRAPPER_H
#include "cpp/MemoryErrorHandling.h"
#include "io_pin_defines.h"

#include <cstdint> // uint8_t, uint16_t, uint32_t

using MEM_ERROR = MemoryErrorHandling::MEM_ERROR;

/**
 * @brief This is an abstract class which defines the functions that must be defined by each hardware interface.
 * For an example see \see SPIWrapper. It's mandatory to implement the Initialize(), SendData() and ReceiveData() methods.
 */
class InterfaceWrapper
{
public:
    virtual ~InterfaceWrapper()= default;

    /**
     * @brief Here the mandatory functions to enable the interface must be executed. Like setting required clocks and ports.
     * @return MEM_ERROR if no error occured otherwise return an error code.
     */
    virtual MEM_ERROR Initialize() = 0;

    /**
     * @brief This function must be implement to send data to the interface.
     * @param data here, the data to send must be passed.
     * @param size here the buffer to send should be passed. After executing the function, this variable contains the amount of actually written bytes.
     * @param timeout a timeout must be specified when using blocking functions.
     * @return MEM_ERROR if no error occured otherwise return an error code.
     */
    virtual MEM_ERROR SendData(uint8_t *data, uint16_t *size, uint32_t timeout) = 0;

    /**
 * @brief This function must be implement to receive data from the interface.
 * @param data the buffer to receive the data. It must be equally or larger the size specified in size.
 * @param size here the buffer to receive should be passed. After executing the function, this variable contains the amount of actually received bytes.
 * @param timeout a timeout must be specified when using blocking functions.
 * @return MEM_ERROR if no error occured otherwise return an error code.
 */
    virtual MEM_ERROR ReceiveData(uint8_t *data, uint16_t *size, uint32_t timeout) = 0;
};


#endif //MEMORY_TESTING_FW_INTERFACEWRAPPER_H
