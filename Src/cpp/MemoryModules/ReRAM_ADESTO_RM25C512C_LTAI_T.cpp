/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include <cstring>
#include "cpp/MemoryModules/ReRAM_ADESTO_RM25C512C_LTAI_T.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "cpp/MemoryControllers/MemoryControllerSPI.h"


RERAM_ADESTO_RM25C512C_LTAI_T::RERAM_ADESTO_RM25C512C_LTAI_T() : MemoryModule(MemoryProperties::TYPE_RERAM,
                                                                              "RERAM_ADESTO_RM25C512C_LTAI_T",
                                                                              MemoryProperties::SPI, 8, 524288,
                                                                              {233, 358}, {3000, 3600})
{
    m_SPIWrapper = new SPIWrapper("SPI5"); // TODO
}

RERAM_ADESTO_RM25C512C_LTAI_T::~RERAM_ADESTO_RM25C512C_LTAI_T()
{
    delete m_SPIWrapper;
}

MEM_ERROR RERAM_ADESTO_RM25C512C_LTAI_T::Initialize()
{
    return MemoryModule::Initialize(m_SPIWrapper);
}

/**
 * @brief Creates the byte buffer containing the command to write from an Adesto RM25C515C chip.
 * The write frame has following format <0x02, second address byte, first address byte, value to write>
 * @param address the address is subdivided into the first and second byte and copied into the sendBuffer.
 * @param valueToWrite the value which should be written to the address.
 * @param returnSendBuffer the buffer containing the commands to send over the SPI interface.
 * @param sendBufferSize the size of the command buffer. To indicate how many bytes should be sent.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
/*static*/ MEM_ERROR RERAM_ADESTO_RM25C512C_LTAI_T::CreateWriteMessage(uint32_t address, uint8_t valueToWrite,
                                                                        uint8_t *returnSendBuffer, uint16_t *sendBufferSize)
{
    if (MemoryController::IsInvalidAddress(address, m_MemorySize))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 4)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    // TODO optimieren
    uint8_t initialize_write_data[] = {MemoryControllerSPI::ReRAM_WRITE, static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF)), valueToWrite};
    memcpy(returnSendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 4;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR RERAM_ADESTO_RM25C512C_LTAI_T::CreateReadMessage(uint32_t address, uint8_t *sendBuffer,
                                                                      uint16_t *sendBufferSize)
{
    if (MemoryController::IsInvalidAddress(address, m_MemorySize))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 3)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    uint8_t initialize_write_data[] = {MemoryControllerSPI::ReRAM_READ, static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF))};
    memcpy(sendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 3;

    return MemoryErrorHandling::MEM_NO_ERROR;
}
