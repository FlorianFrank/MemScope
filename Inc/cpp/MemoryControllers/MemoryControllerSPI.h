/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MEMORY_TESTING_FW_MEMORYCONTROLLERSPI_H
#define MEMORY_TESTING_FW_MEMORYCONTROLLERSPI_H

#include "cpp/InterfaceWrappers/SPIWrapper.h"
#include "MemoryController.h"

/**
 * @brief This class implements the communication with SPI based memory controllers.
 * Currently the Fujitsu ReRAM MB85AS4MT an the ReRAM_Adesto_RM25C512C is supported.
 */
class MemoryControllerSPI : public MemoryController
{
public:
    explicit MemoryControllerSPI(SPIWrapper *interfaceWrapper, MemoryModule &memoryModule);

    enum
    {
        /** Set Write Enable Latch */
        ReRAM_WREN = (uint8_t) 0x06,
        /** Reset Write Enable Latch */
        ReRAM_WRDI = (uint8_t) 0x04,
        /** Read Status Register */
        ReRAM_RDSR = (uint8_t) 0x05,
        /** Write Status Register */
        ReRAM_WRSR = (uint8_t) 0x01,
        /** Read Memory Code */
        ReRAM_READ = (uint8_t) 0x03,
        /** Write Memory Code */
        ReRAM_WRITE = (uint8_t) 0x02,

        ReRAM_PD        = (uint8_t) 0x09,       // TODO CHECK
        /** Ultra Deep Power Down */
        ReRAM_UDPD = (uint8_t) 0x79,
        /** Resume from Power Down */
        ReRAM_RES = (uint8_t) 0xAB,
        /** Read Device ID */
        ReRAM_RDID = (uint8_t) 0x9F,
        /** Sleep Mode */
        ReRAM_SLEEP = (uint8_t) 0xB9,
        /** allows the user to reduce the power of the device to its lowest power consumption state. */
        ReRam_PowerDown = (uint8_t) 0xAB
    } typedef SPI_Commands;

    /**
     * @brief This struct defines each register bit of the registers of the Adesto and Fujitsu ReRAM chip.
     */
    struct
    {
        /** This register protects writing to a status register */
        uint8_t WriteProtectPin;
        /** This register protects writing to a status register */
        uint8_t AutoPowerDownEnable;
        uint8_t LowPowerStandbyEnable;
        /** This defines size of write protect block for the WRITE command  */
        uint8_t BlockProtectionBits;
        /** This indicates ReRAM Array and status register are writable. */
        uint8_t WriteEnableBit;
        /** This indicates ReRAM Array and status register are in writing process. */
        uint8_t WriteInProgressBit;
    } typedef MemoryStatusRegister;

    // Basic read and write functions
    MEM_ERROR Write8BitWord(uint32_t address, uint8_t valueToWrite) override;
    MEM_ERROR Read8BitWord(uint32_t address, uint8_t *readValue) override;
    MEM_ERROR Write16BitWord(uint32_t address, uint16_t valueToWrite) override;
    MEM_ERROR Read16BitWord(uint32_t address, uint16_t *valueToRead) override;

    // Status register functions
    MEM_ERROR WriteStatusRegister(MemoryStatusRegister &statusRegister);
    MEM_ERROR ReadStatusRegister(MemoryStatusRegister &statusRegister);
    static MemoryStatusRegister Uint8ToStatusRegister(uint8_t statusRegister);
    static uint8_t StatusRegisterToUint8(MemoryStatusRegister &memoryStatusRegister);
    static void PrintStatusRegister(MemoryStatusRegister reg);

    MEM_ERROR SetWriteEnableLatch(bool checkRegister, uint32_t timeoutInMs);
    MEM_ERROR Reset_WriteEnableLatch();
    uint32_t PollWriteInProgressRegister(uint32_t timeoutCycles);
    MEM_ERROR SetPowerDown();
    MEM_ERROR ResumeFromPowerDown();
    MEM_ERROR SetUltraDeepPowerDown();
    MEM_ERROR SetSleepMode();
    MEM_ERROR EraseChip();

    // Helper functions
    MEM_ERROR SendSPICommand(SPI_Commands spiCMD, uint8_t *retValue, bool response);

private:
    SPIWrapper *m_SPIWrapper;
};
#endif //MEMORY_TESTING_FW_MEMORYCONTROLLERSPI_H
