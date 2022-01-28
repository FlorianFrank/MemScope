/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of Computer Engineering
 */
#include <cstring>
#include "cpp/MemoryModules/ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "cpp/MemoryControllers/MemoryControllerSPI.h"

ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1(): MemoryModule(MemoryProperties::TYPE_RERAM,
                                                                                       "ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1",
                                                                                       MemoryProperties::SPI, 8, 524288, {233, 358},
                                                                                       {3000, 3600})
{
    m_SPIWrapper = new SPIWrapper("SPI5");
}

ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::~ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1()
{
    delete m_SPIWrapper;
}

MEM_ERROR ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::Initialize()
{
    return MemoryModule::Initialize(m_SPIWrapper);
}

/**
 * @brief Creates the byte buffer containing the command to write from an Fujitsu MB85AS4M ReRAM chip.
 * The read frame has following format <0x02, third address byte, second address byte, first address byte, valueToWrite to send>
 * @param address the address is subdivided into the first, second and third byte and copied into the sendBuffer.
 * @param valueToWrite the value which should be written on the address.
 * @param sendBuffer the buffer containing the commands to send over the SPI interface.
 * @param sendBufferSize the size of the command buffer. To indicate how many bytes should be sent.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
MEM_ERROR ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::CreateWriteMessage(uint32_t address, uint8_t valueToWrite,
                                                                 uint8_t *returnSendBuffer, uint16_t *sendBufferSize)
{
    if (MemoryController::IsInvalidAddress(address, m_MemorySize))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 5)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    uint8_t initialize_write_data[] = {MemoryControllerSPI::ReRAM_WRITE, static_cast<uint8_t>(((address >> 16) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >>  0) & 0xFF)), valueToWrite};

    memcpy(returnSendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 5;

    return MemoryErrorHandling::MEM_NO_ERROR;
}


/**
 * @brief Creates the byte buffer containing the command to read from an Fujitsu MB85AS4M ReRAM chip.
 * The read frame has following format <0x03, third address byte, second address byte, first address byte>
 * @param address the address is subdivided into the first, second and third byte and copied into the sendBuffer.
 * @param sendBuffer the buffer containing the commands to send over the SPI interface.
 * @param sendBufferSize the size of the command buffer. To indicate how many bytes should be sent.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
MEM_ERROR ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1::CreateReadMessage(uint32_t address, uint8_t *sendBuffer, uint16_t *sendBufferSize)
{
    if (MemoryController::IsInvalidAddress(address, m_MemorySize))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 4)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    uint8_t initialize_write_data[] = {MemoryControllerSPI::ReRAM_READ, static_cast<uint8_t>(((address >> 16) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF))};

    memcpy(sendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 4;

    return MemoryErrorHandling::MEM_NO_ERROR;
}