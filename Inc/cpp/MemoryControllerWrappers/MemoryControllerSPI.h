//
// Created by florianfrank on 07.12.21.
//

#ifndef MEMORY_TESTING_FW_MEMORYCONTROLLERSPI_H
#define MEMORY_TESTING_FW_MEMORYCONTROLLERSPI_H

#include <cpp/InterfaceWrappers/SPIWrapper.h>
#include "cpp/MemoryController.h"

class MemoryControllerSPI: public MemoryController
{
public:
    explicit MemoryControllerSPI(SPIWrapper *interfaceWrapper);

    enum
    {
        ReRAM_WREN      = (uint8_t) 0x06,       // Set Write Enable Latch
        ReRAM_WRDI      = (uint8_t) 0x04,       // Reset Write Enable Latch
        ReRAM_RDSR      = (uint8_t) 0x05,       // Read Status Register
        ReRAM_WRSR      = (uint8_t) 0x01,       // Write Status Register
        ReRAM_READ      = (uint8_t) 0x03,       // Read Memory Code
        ReRAM_WRITE     = (uint8_t) 0x02,       // Write Memory Code
        ReRAM_PD        = (uint8_t) 0x09,       // Chip Erase
        ReRAM_UDPD      = (uint8_t) 0x79,       // Ultra Deep Power Down
        ReRAM_RES       = (uint8_t) 0xAB,       // Resume from Power Down
        ReRAM_RDID      = (uint8_t) 0b10011111, // Read Device ID
        ReRAM_SLEEP     = (uint8_t) 0b10111001, // Sleep Mode
        ReRam_PowerDown = (uint8_t) 0xB9        //
    } typedef SPI_Commands;

    struct
    {
        uint8_t wp_enable_Pin;
        uint8_t auto_power_down_enable;
        uint8_t low_power_standby_enable;
        uint8_t block_protection_bits;
        uint8_t write_enable_bit;
        uint8_t write_in_progress_bit;

    } typedef MemoryStatusRegister;

    // Basic read and write functions
    MEM_ERROR Write8BitWord(uint32_t adr, uint8_t value) override;
    MEM_ERROR Read8BitWord(uint32_t adr, uint8_t *ret) override;
    MEM_ERROR Write16BitWord(uint32_t adr, uint16_t value) override;
    MEM_ERROR Read16BitWord(uint32_t adr, uint16_t *value) override;

    MEM_ERROR WriteStatusRegister(MemoryStatusRegister *statusRegister, uint8_t value);
    MEM_ERROR ReadStatusRegister(MemoryStatusRegister *statusRegister);
    static MemoryStatusRegister ParseStatusRegister(uint8_t statusRegister);
    static void PrintStatusRegister(MemoryStatusRegister reg);
    uint32_t WIP_Polling(uint32_t timeoutCycles);

    MEM_ERROR Set_WriteEnableLatch(bool checkRegister);
    MEM_ERROR Reset_WriteEnableLatch();

    MEM_ERROR SetPowerDown();
    MEM_ERROR SetUltraDeepPowerDown();
    MEM_ERROR ResumeFromPowerDown();
    MEM_ERROR SetSleepMode();

    MEM_ERROR EraseChip();

    MEM_ERROR SendSPICommand(SPI_Commands spiCMD, uint8_t *retValue, bool response);
    // Memory specific functions
    static MEM_ERROR CreateWriteMessageReRAMAdesto(uint32_t address, uint8_t value, uint8_t *retValue, uint16_t *retSize);
    static MEM_ERROR CreateWriteMessageReRAMFujitsu(uint32_t address, uint8_t value, uint8_t *retValue, uint16_t *retSize);


    static MEM_ERROR CreateReadMessageReRAMAdesto(uint32_t address, uint8_t *retMessage, uint16_t *inputOuputSize);

    static MEM_ERROR CreateReadMessageReRAMFujitsu(uint32_t address, uint8_t *retMessage, uint16_t *inputOuputSize);


private:
    SPIWrapper *m_SPIWrapper;
};


#endif //MEMORY_TESTING_FW_MEMORYCONTROLLERSPI_H
