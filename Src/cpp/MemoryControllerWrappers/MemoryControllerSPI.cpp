//
// Created by florianfrank on 07.12.21.
//

#include <cstring> // memcpy
#include <cstdio> // printf TODO remove later
#include "cpp/MemoryControllerWrappers/MemoryControllerSPI.h"
#include "cpp/InterfaceWrappers/SPIWrapper.h"


MemoryControllerSPI::MemoryControllerSPI(InterfaceWrappers &interfaceWrapper)
        : MemoryController(interfaceWrapper)
{
    m_SPIWrapper = new SPIWrapper;
}

MEM_ERROR MemoryControllerSPI::MemoryWrite8BitWord(uint32_t adr, uint8_t value)
{

MEM_ERROR err = Set_WriteEnableLatch(false);
    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;

// TODO
#define RERAM_ADESTO_RM25C512C_LTAI_T 1
    // Write Execution
#if RERAM_ADESTO_RM25C512C_LTAI_T

    uint8_t initialize_write_data[8];
    uint16_t buffer_len = 8;
    // Use 2 address bytes
    MEM_ERROR ret =  CreateWriteMessageReRAMAdesto(adr, value, initialize_write_data, &buffer_len);
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    // TODO ableitung memory type
#elif RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
    // Use 3 address bytes
	uint8_t initialize_write_data[] = {ReRAM_WRITE,((adr >> 16) & 0xFF), ((adr >> 8) & 0xFF), ((adr >>  0) & 0xFF), value};
#else
	return MEM_INTERFACE_NOT_SUPPORTED;
#endif // if RERAM_ADESTO_RM25C512C_LTAI_T else if FRAM_FUJITSU_MB85R1001ANC_GE1

    SPIWrapper::SetWriteProtect();
    SPIWrapper::SetChipSelect();
    err = m_SPIWrapper->SendData(initialize_write_data, &buffer_len, 10000);
    SPIWrapper::ResetChipSelect();
    SPIWrapper::ResetWriteProtect();

    if(err != MemoryErrorHandling::MEM_NO_ERROR)
        return err;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerSPI::MemoryRead8BitWord(uint32_t adr, uint8_t *ret) const
{
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerSPI::MemoryWrite16BitWord(uint32_t adr, uint16_t value)
{
    return MemoryErrorHandling::MEM_INVALID_COMMAND;
}

MEM_ERROR MemoryControllerSPI::MemoryRead16BitWord(uint32_t adr, uint16_t *value) const
{
    return MemoryErrorHandling::MEM_INVALID_COMMAND;
}

// TODO remove
#define ReRAM_WRITE     (uint8_t) 0x02       // Write Memory Code


/*static*/ MEM_ERROR MemoryControllerSPI::CreateWriteMessageReRAMAdesto(uint32_t address, uint8_t value,
                                                                        uint8_t *retValue, uint16_t *retSize)
{
    if (isInvalidAddress(address))
        return MemoryErrorHandling::MEM_INVALID_ADDRESS;

    if (!retSize || *retSize < 4)
        return MemoryErrorHandling::MEM_BUFFER_TO_SMALL;

        // TODO optimieren
    uint8_t initialize_write_data[] = {ReRAM_WRITE, static_cast<uint8_t>(((address >> 8) & 0xFF)),
                                       static_cast<uint8_t>(((address >> 0) & 0xFF)), value};
    memcpy(retValue, initialize_write_data, sizeof initialize_write_data);
    *retSize = 4;

    return MemoryErrorHandling::MEM_NO_ERROR;
}

uint32_t MemoryControllerSPI::WIP_Polling(uint32_t timeoutCycles)
{
    uint32_t startTS;
    uint32_t endTS = 0;

    //start timer
    TimeMeasurement::ResetAndStartTimer();
    startTS = TimeMeasurement::GetTimer();

    // TODO timeout
    do {
        MemoryStatusRegister statusRegister;
        MEM_ERROR err = ReadStatusRegister(&statusRegister);
        if(err != MemoryErrorHandling::MEM_NO_ERROR )
            return err;

        if (statusRegister.write_in_progress_bit == 0) {
            TimeMeasurement::StopTimer();
            endTS = TimeMeasurement::GetTimer() - startTS;
            return endTS;
        }
        if(timeoutCycles != 0)
            endTS = TimeMeasurement::GetTimer();
    }while(startTS + timeoutCycles > endTS);

    return endTS;
}

MEM_ERROR MemoryControllerSPI::EraseChip()
{
    return SendSPICommand(ReRAM_PD, nullptr, false);
}

MEM_ERROR MemoryControllerSPI::SendSPICommand(SPI_Commands spiCMD, uint8_t *retValue, bool response) {
    if(response && !retValue)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    auto cmd = (uint8_t)spiCMD;
    SPIWrapper::SetChipSelect();
    uint16_t size = 1;
    MEM_ERROR err = m_SPIWrapper->SendData(&cmd, &size, 10);
    size = 1;
    if(response || err != MemoryErrorHandling::MEM_NO_ERROR)
        m_SPIWrapper->SendData(retValue, &size, 10);
    SPIWrapper::ResetChipSelect();
    return err;

}

/*static*/ MemoryControllerSPI::MemoryStatusRegister MemoryControllerSPI::ParseStatusRegister(uint8_t statusRegister)
{
    return (MemoryStatusRegister) {static_cast<uint8_t>((statusRegister & 128) >> 7),
                                   static_cast<uint8_t>((statusRegister & 64) >> 6),
                                   static_cast<uint8_t>((statusRegister & 32) >> 5),
                                   static_cast<uint8_t>((statusRegister & 12) >> 3),
                                   static_cast<uint8_t>((statusRegister & 2) >> 1),
                                   static_cast<uint8_t>((statusRegister & 1))};
}

void MemoryControllerSPI::PrintStatusRegister(MemoryStatusRegister reg)
{

    printf("WP_Enable: %d\n"
           "Auto_Power_Down_Enable: %d\n"
           "Low_Power_Standby_Enable: %d\n"
           "Block_Protection_Bits: %d\n"
           "Write_Enable_Bits: %d\n"
           "Write_In_Progress_Bits: %d\n", reg.wp_enable_Pin, reg.auto_power_down_enable, reg.low_power_standby_enable,
           reg.block_protection_bits, reg.write_enable_bit, reg.write_in_progress_bit);
}

MEM_ERROR MemoryControllerSPI::WriteStatusRegister(MemoryStatusRegister *statusRegister, uint8_t value)
{
    if (!statusRegister)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    uint8_t reg = value;
    MEM_ERROR ret = SendSPICommand(ReRAM_WRSR, &reg, true);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    *statusRegister = ParseStatusRegister(reg);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerSPI::SetPowerDown()
{
    return SendSPICommand(ReRam_PowerDown, nullptr, false);
}

MEM_ERROR MemoryControllerSPI::Set_WriteEnableLatch(bool checkRegister)
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
            MEM_ERROR err = ReadStatusRegister(&statusRegister);
            if (err != MemoryErrorHandling::MEM_NO_ERROR)
                return err;
        }while(statusRegister.write_enable_bit != 1);
        //return MEM_REGISTER_NOT_SET;
    }
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerSPI::ReadStatusRegister(MemoryStatusRegister *statusRegister)
{
    if (!statusRegister)
        return MemoryErrorHandling::MEM_INVALID_ARGUMENT;

    uint8_t reg = 0;
    MEM_ERROR ret = SendSPICommand(ReRAM_RDSR, &reg, true);
    if (ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    *statusRegister = ParseStatusRegister(reg);
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerSPI::Reset_WriteEnableLatch()
{
    SPIWrapper::SetWriteProtect();
    MEM_ERROR ret = SendSPICommand(ReRAM_WRDI, nullptr, false);
    SPIWrapper::ResetWriteProtect();
    if(ret != MemoryErrorHandling::MEM_NO_ERROR)
        return ret;
    return MemoryErrorHandling::MEM_NO_ERROR;
}

MEM_ERROR MemoryControllerSPI::ResumeFromPowerDown()
{
    return SendSPICommand(ReRAM_RES, NULL, false);
}

MEM_ERROR MemoryControllerSPI::SetUltraDeepPowerDown()
{
    return SendSPICommand(ReRAM_UDPD, NULL, false);
}

MEM_ERROR MemoryControllerSPI::SetSleepMode()
{
    return SendSPICommand(ReRAM_SLEEP, NULL, false);
}
