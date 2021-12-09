/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#include <cstring> // memcpy
#include <cstdio> // printf TODO remove later
#include "cpp/MemoryControllerWrappers/MemoryControllerSPI.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"


MemoryControllerSPI::MemoryControllerSPI(SPIWrapper *interfaceWrapper):
MemoryController(interfaceWrapper), m_SPIWrapper(interfaceWrapper)
{
}

/**
 * @brief Writes a single 8 bit valueToWrite to an address.
 * @param address address to write.
 * @param valueToWrite valueToWrite to write.
 * @return MEM_NO_ERROR is write operation was successful otherwise an error code is returned.
 */
MEM_ERROR MemoryControllerSPI::Write8BitWord(uint32_t address, uint8_t valueToWrite)
{
    MEM_ERROR err = SetWriteEnableLatch(false);
    if (err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    uint8_t sendBuffer[8];
    uint16_t sendBufferLen = 8;

    MEM_ERROR ret = MemoryErrorHandling::MEM_NO_ERROR;
#if RERAM_ADESTO_RM25C512C_LTAI_T
    ret =  CreateWriteMessageReRAMAdesto(address, valueToWrite, sendBuffer, &sendBufferLen);
#elif RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
    ret = CreateWriteMessageReRAMFujitsu(address, valueToWrite, sendBuffer, &sendBufferLen);
#else // unsupported memory type
    return MemoryErrorHandling::MEM_MEMORY_NOT_SUPPORTED;
#endif // if RERAM_ADESTO_RM25C512C_LTAI_T

    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    SPIWrapper::SetWriteProtect();
    SPIWrapper::SetChipSelect();
    err = m_SPIWrapper->SendData(sendBuffer, &sendBufferLen, 10000);
    SPIWrapper::ResetChipSelect();
    SPIWrapper::ResetWriteProtect();

    if (err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Reads a single 8 bit value to an address.
 * @param address address from which data should be read.
 * @param readValue return value containing the read value.
 * @return MEM_NO_ERROR is write operation was successful otherwise an error code is returned.
 */
MEM_ERROR MemoryControllerSPI::Read8BitWord(uint32_t address, uint8_t *readValue)
{
    MEM_ERROR err = SetWriteEnableLatch(false);
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    uint8_t readData[4];
    uint16_t lenReadData = 4;

    // Write Execution
#if RERAM_ADESTO_RM25C512C_LTAI_T
    err = CreateReadMessageReRAMAdesto(address, readData, &lenReadData);
#elif RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
    err =  CreateReadMessageReRAMFujitsu(address, readData, &lenReadData);
#else // if RERAM_ADESTO_RM25C512C_LTAI_T
    return MemoryErrorHandling::MEM_MEMORY_NOT_SUPPORTED;
#endif // if RERAM_ADESTO_RM25C512C_LTAI_T else if FRAM_FUJITSU_MB85R1001ANC_GE1
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    SPIWrapper::SetChipSelect();
    SPIWrapper::SetWriteProtect();
    err = m_SPIWrapper->SendData(readData, &lenReadData, 1000);
    SPIWrapper::ResetWriteProtect();
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;


    uint8_t retData[2];
    uint16_t retDataSize = 2;
    err = m_SPIWrapper->ReceiveData(retData, &retDataSize, 1000);
    SPIWrapper::ResetChipSelect();
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Function not supported!
 * @param address not used
 * @param valueToWrite not used
 * @return MEM_INVALID_COMMAND
 */
MEM_ERROR MemoryControllerSPI::Write16BitWord(uint32_t address, uint16_t valueToWrite)
{
    return MemoryErrorHandling::MEM_INVALID_COMMAND;
}

/**
 * @brief Function not supported!
 * @param address not used
 * @param valueToRead not used
 * @return MEM_INVALID_COMMAND
 */
MEM_ERROR MemoryControllerSPI::Read16BitWord(uint32_t address, uint16_t *valueToRead)
{
    return MemoryErrorHandling::MEM_INVALID_COMMAND;
}

/**
 * @brief This function writes the specified status register to the Memory chip.
 * @param statusRegister the register which should be written.
 * @return MEM_NO_ERROR is write operation was successful otherwise an error code is returned.
 */
MEM_ERROR MemoryControllerSPI::WriteStatusRegister(MemoryStatusRegister &statusRegister)
{

    uint8_t reg = StatusRegisterToUint8(statusRegister);
    MEM_ERROR ret = SendSPICommand(ReRAM_WRSR, &reg, true);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function reads the status register from the memory chip.
 * @param statusRegister after a successfull execution, this variable contains the returned status register.
 * @return MEM_NO_ERROR is write operation was successful otherwise an error code is returned.
 */
MEM_ERROR MemoryControllerSPI::ReadStatusRegister(MemoryStatusRegister &statusRegister)
{
    uint8_t reg = 0;
    MEM_ERROR ret = SendSPICommand(ReRAM_RDSR, &reg, true);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    statusRegister = Uint8ToStatusRegister(reg);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/*static*/ MemoryControllerSPI::MemoryStatusRegister MemoryControllerSPI::Uint8ToStatusRegister(uint8_t statusRegister)
{
    return (MemoryStatusRegister) {static_cast<uint8_t>((statusRegister & 128) >> 7),
                                   static_cast<uint8_t>((statusRegister & 64) >> 6),
                                   static_cast<uint8_t>((statusRegister & 32) >> 5),
                                   static_cast<uint8_t>((statusRegister & 12) >> 2),
                                   static_cast<uint8_t>((statusRegister & 2) >> 1),
                                   static_cast<uint8_t>((statusRegister & 1))};
}

/**
 * @brief This function returns the statusregister struct into one byte which can be send to the memory.
 * @param memoryStatusRegister the status register which is transformed into byte representation.
 * @return the status register in byte representation.
 */
/*static*/ uint8_t MemoryControllerSPI::StatusRegisterToUint8(MemoryStatusRegister &memoryStatusRegister)
{
    uint8_t retValue = 0;
    if (memoryStatusRegister.WriteProtectPin != 0)
        retValue += 0x80;
    if (memoryStatusRegister.AutoPowerDownEnable != 0)
        retValue += 0x40;
    if (memoryStatusRegister.LowPowerStandbyEnable != 0)
        retValue += 0x20;
    if (memoryStatusRegister.BlockProtectionBits != 0)
        retValue += memoryStatusRegister.BlockProtectionBits;
    if (memoryStatusRegister.WriteEnableBit != 0)
        retValue += 0x02;
    if (memoryStatusRegister.WriteEnableBit != 0)
        retValue += 0x01;
    return retValue;
}

/**
 * @brief Prints the status register TODO this should be implementet in a special formatter class.
 + @param reg the status register which should be printed on the command line.
 */
void MemoryControllerSPI::PrintStatusRegister(MemoryStatusRegister reg)
{

    printf("WP_Enable: %d\n"
           "Auto_Power_Down_Enable: %d\n"
           "Low_Power_Standby_Enable: %d\n"
           "Block_Protection_Bits: %d\n"
           "Write_Enable_Bits: %d\n"
           "Write_In_Progress_Bits: %d\n", reg.WriteProtectPin, reg.AutoPowerDownEnable, reg.LowPowerStandbyEnable,
           reg.BlockProtectionBits, reg.WriteEnableBit, reg.WriteInProgressBit);
}

/**
 * @brief This function sets the write enable latch register.
 * @param checkRegister flag if the function should check if the register is set.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
MEM_ERROR MemoryControllerSPI::SetWriteEnableLatch(bool checkRegister)
{
    SPIWrapper::SetWriteProtect();
    MEM_ERROR ret = SendSPICommand(ReRAM_WREN, nullptr, false);
    SPIWrapper::ResetWriteProtect();
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;

    if(checkRegister)
    {
        MemoryStatusRegister statusRegister;
        do
        {
            MEM_ERROR err = ReadStatusRegister(statusRegister);
            if (err != MemoryErrorHandling::MEM_NO_ERROR)
                return err;
            // TODO timeout
        }while(statusRegister.WriteEnableBit != 1);
        //return MEM_REGISTER_NOT_SET;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This function resets the write enable latch register.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
MEM_ERROR MemoryControllerSPI::Reset_WriteEnableLatch()
{
    SPIWrapper::SetWriteProtect();
    MEM_ERROR ret = SendSPICommand(ReRAM_WRDI, nullptr, false);
    SPIWrapper::ResetWriteProtect();
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief This method can be executed after a write operation.
 * The write operation sets the write in progress bit to 1.
 * This method uses busy waiting to constantly poll this register until it changes to 0, which means
 * the write operation has finished.
 * @param timeoutCycles in case of an error and the WIP register doesn't change, this function returns with an error.
 * @return the amount of clock cycles required for WIP polling to flip from 1 to zero
 */
uint32_t MemoryControllerSPI::PollWriteInProgressRegister(uint32_t timeoutCycles)
{
    uint32_t startTS;
    uint32_t endTS = 0;

    //start timer
    TimeMeasurement::ResetAndStartTimer();
    startTS = TimeMeasurement::GetTimer();

    // TODO timeout
    do {
        MemoryStatusRegister statusRegister;
        MEM_ERROR err = ReadStatusRegister(statusRegister);
        if(err != MemoryErrorHandling::MEM_NO_ERROR )
            return err;

        if (statusRegister.WriteInProgressBit == 0) {
            TimeMeasurement::StopTimer();
            endTS = TimeMeasurement::GetTimer() - startTS;
            return endTS;
        }
        if(timeoutCycles != 0)
            endTS = TimeMeasurement::GetTimer();
    }while(startTS + timeoutCycles > endTS);

    return endTS;
}

/**
 * @brief Power Down mode allows the user to reduce the power of the device to its lowest power consumption state.
 * All instructions given during the Power Down mode are ignored except the Resume from Power down (RES) instruction.
 * Therefore this mode can be used as an additional software write protection feature.
 * @return MEM_NO_ERROR if the command was sent sucessfully.
 */
MEM_ERROR MemoryControllerSPI::SetPowerDown()
{
    return SendSPICommand(ReRam_PowerDown, nullptr, false);
}

/**
 * @brief The Resume from Power Down mode is the only command that will wake the device up from the Power Down mode. All
 * other commands are ignored. In the simple instruction command, after the CS pin is brought low, the RES instruction
 * is shifted in. At the end of the instruction, the CS pin is brought back high.
 * @return MEM_NO_ERROR if the command was sent successfully.
 */
MEM_ERROR MemoryControllerSPI::ResumeFromPowerDown()
{
    return SendSPICommand(ReRAM_RES, nullptr, false);
}

/**
 * @brief he Ultra-Deep Power Down mode allows the device to further reduce its energy consumption compared to the existing
 * Standby and Power Down modes by shutting down additional internal circuitry. When the device is in the Ultra-Deep
 * Power Down mode, all commands including the Read Status Register and Resume from Deep Power Down commands
 * will be ignored. Since all commands will be ignored, the mode can be used as an extra protection mechanism against
 * inadvertent or unintentional program and erase operations. Entering the Ultra-Deep Power Down mode is accomplished
 * by simply asserting the CS pin, clocking in the opcode 79h, and then deasserting the CS pin. Any additional data clocked
 * into the device after the opcode will be ignored. When the CS pin is deasserted, the device will enter the Ultra-Deep
 * Power Down mode within the maximum time of t EUDPD .
 * @return MEM_NO_ERROR if the command was sent successfully.
 */
MEM_ERROR MemoryControllerSPI::SetUltraDeepPowerDown()
{
    return SendSPICommand(ReRAM_UDPD, nullptr, false);
}

// TODO remove?
MEM_ERROR MemoryControllerSPI::SetSleepMode()
{
    return SendSPICommand(ReRAM_SLEEP, nullptr, false);
}

/**
 * @brief Chip Erase sets all bits inside the device to a 1. A Write Enable (WREN) instruction is required prior to a Chip Erase.
 * After the WREN instruction is shifted in, the CS pin must be brought high to set the Write Enable Latch.
 * @return MEM_NO_ERROR if the command was sent successfully.
 */
MEM_ERROR MemoryControllerSPI::EraseChip()
{
    return SendSPICommand(ReRAM_PD, nullptr, false);
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
/*static*/ MEM_ERROR MemoryControllerSPI::CreateWriteMessageReRAMAdesto(uint32_t address, uint8_t valueToWrite,
                                                                        uint8_t *returnSendBuffer, uint16_t *sendBufferSize)
{
    if (IsInvalidAddress(address))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 4)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    // TODO optimieren
    uint8_t initialize_write_data[] = {ReRAM_WRITE, static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF)), valueToWrite};
    memcpy(returnSendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 4;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Creates the byte buffer containing the command to read from an Adesto RM25C515C chip.
 * The read frame has following format <0x03, second address byte, first address byte>
 * @param address the address is subdivided into the first and second byte and copied into the sendBuffer.
 * @param sendBuffer the buffer containing the commands to send over the SPI interface.
 * @param sendBufferSize the size of the command buffer. To indicate how many bytes should be sent.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
/*static*/ MEM_ERROR MemoryControllerSPI::CreateReadMessageReRAMAdesto(uint32_t address, uint8_t *sendBuffer, uint16_t *sendBufferSize)
{
    if (IsInvalidAddress(address))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 3)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    uint8_t initialize_write_data[] = {ReRAM_READ, static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF))};
    memcpy(sendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 3;

    return MemoryErrorHandling::MEM_NO_ERROR;
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
/*static*/ MEM_ERROR MemoryControllerSPI::CreateWriteMessageReRAMFujitsu(uint32_t address, uint8_t valueToWrite,
                                                                         uint8_t *sendBuffer, uint16_t *sendBufferSize)
{
    if (IsInvalidAddress(address))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 5)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    uint8_t initialize_write_data[] = {ReRAM_WRITE, static_cast<uint8_t>(((address >> 16) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >>  0) & 0xFF)), valueToWrite};

    memcpy(sendBuffer, initialize_write_data, sizeof initialize_write_data);
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
/*static*/ MEM_ERROR MemoryControllerSPI::CreateReadMessageReRAMFujitsu(uint32_t address, uint8_t *sendBuffer, uint16_t *sendBufferSize)
{
    if (IsInvalidAddress(address))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!sendBufferSize || *sendBufferSize < 4)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

    uint8_t initialize_write_data[] = {ReRAM_READ, static_cast<uint8_t>(((address >> 16) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF))};

    memcpy(sendBuffer, initialize_write_data, sizeof initialize_write_data);
    *sendBufferSize = 4;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

/**
 * @brief Helper function which sends a simple command to the SPI interface. These commands only have one byte of payload,
 * for example Resume from Power Down.
 * @param spiCMD command of the memory which should be sent.
 * @param retValue return value from the memory chip if the response flag is set.
 * @param response indicates if the function should wait for an response.
 * @return MEM_NO_ERROR if no error occurred otherwise return error code.
 */
MEM_ERROR MemoryControllerSPI::SendSPICommand(SPI_Commands spiCMD, uint8_t *retValue, bool response) {
    if(response && !retValue)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    auto cmd = (uint8_t)spiCMD;
    SPIWrapper::SetChipSelect();
    uint16_t size = 1;
    MEM_ERROR err = m_SPIWrapper->SendData(&cmd, &size, 10);
    size = 1;
    if(response || err != MemoryErrorHandling::MEM_NO_ERROR)
        m_SPIWrapper->ReceiveData(retValue, &size, 10);
    SPIWrapper::ResetChipSelect();
    return err;

}

