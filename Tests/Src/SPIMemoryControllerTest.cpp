#include <gtest/gtest.h>
#include <cpp/MemoryControllers/MemoryControllerSPI.h>
#include "cpp/MemoryErrorHandling.h"

#include "cpp/MemoryModules/ReRAM_ADESTO_RM25C512C_LTAI_T.h"
#include "cpp/MemoryModules/ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1.h"

TEST(MemoryControllerTestSPI, TestWriteAdesto)
{
    uint8_t retValue[4];
    uint16_t retSize = 4;

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MEM_ERROR ret = memoryModule.CreateWriteMessage(0x55AA, 0xFB, retValue, &retSize);

    EXPECT_EQ(ret,MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(retSize, 4);
    EXPECT_EQ(retValue[0], 0x02);
    EXPECT_EQ(retValue[1], 0x55);
    EXPECT_EQ(retValue[2], 0xAA);
    EXPECT_EQ(retValue[3], 0xFB);
}

TEST(MemoryControllerTestSPI, TestWriteFujitsu)
{
    uint8_t retValue[5];
    uint16_t retSize = 5;

    ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1 memoryModule;
    MEM_ERROR ret = memoryModule.CreateWriteMessage(0x002233, 0xFB, retValue, &retSize);

    EXPECT_EQ(ret,MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(retSize, 5);
    EXPECT_EQ(retValue[0], 0x02);
    EXPECT_EQ(retValue[1], 0x00);
    EXPECT_EQ(retValue[2], 0x22);
    EXPECT_EQ(retValue[3], 0x33);
    EXPECT_EQ(retValue[4], 0xFB);
}

TEST(MemoryControllerTestSPI, TestReadAdesto)
{

    uint8_t retValue[5];
    uint16_t retSize = 5;
    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;

    MEM_ERROR ret = memoryModule.CreateReadMessage(0x55AA, retValue, &retSize);

    EXPECT_EQ(ret,MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(retSize, 3);
    EXPECT_EQ(retValue[0], 0x03);
    EXPECT_EQ(retValue[1], 0x55);
    EXPECT_EQ(retValue[2], 0xAA);
}

TEST(MemoryControllerTestSPI, TestReadFujitsu)
{

    uint8_t retValue[5];
    uint16_t retSize = 5;

    ReRAM_Fujitsu_MB85AS4MTPF_G_BCERE1 memoryModule;
    MEM_ERROR ret = memoryModule.CreateReadMessage(0x00AABB, retValue, &retSize);

    EXPECT_EQ(ret,MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(retSize, 4);
    EXPECT_EQ(retValue[0], 0x03);
    EXPECT_EQ(retValue[1], 0x00);
    EXPECT_EQ(retValue[2], 0xAA);
    EXPECT_EQ(retValue[3], 0xBB);
}



TEST(MemoryControllerTestSPI, TestWrite8BitWord)
{
    SPIWrapper spiWrapper("SPI1");
    RERAM_ADESTO_RM25C512C_LTAI_T reramAdestoRm25C512CLtaiT;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, reramAdestoRm25C512CLtaiT);
    MEM_ERROR err = memoryControllerSpi.Write16BitWord(0, 0);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_INVALID_COMMAND);

    uint16_t value;
    err = memoryControllerSpi.Read16BitWord(0, &value);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_INVALID_COMMAND);

}

TEST(MemoryControllerTestSPI, TestInvalidArgument)
{
    uint8_t retValue[3];
    uint16_t retSize = 3;

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;

    MEM_ERROR ret = memoryModule.CreateWriteMessage(0x55AA, 0xFB, retValue, &retSize);
    EXPECT_EQ(ret,MemoryErrorHandling::MEM_BUFFER_TO_SMALL);

    ret = memoryModule.CreateWriteMessage(0xAAAAAAAA, 0xFB, retValue, &retSize);
    EXPECT_EQ(ret,MemoryErrorHandling::MEM_INVALID_ADDRESS);
}

TEST(MemoryControllerTestSPI, TestInvalidWriteReadCommands)
{
    SPIWrapper spiWrapper("SPI1");
    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MEM_ERROR err = memoryControllerSpi.Write16BitWord(0, 0);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_INVALID_COMMAND);

    uint16_t value;
    err = memoryControllerSpi.Read16BitWord(0, &value);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_INVALID_COMMAND);
}


TEST(MemoryControllerTestSPI, ParseStatusRegister)
{
    uint8_t registerValue = 0x9D; // 10011101
    MemoryControllerSPI::MemoryStatusRegister memoryStatusRegister = MemoryControllerSPI::Uint8ToStatusRegister(registerValue);

    EXPECT_EQ(memoryStatusRegister.WriteInProgressBit, 1);
    EXPECT_EQ(memoryStatusRegister.WriteEnableBit, 0);
    EXPECT_EQ(memoryStatusRegister.BlockProtectionBits, 3);
    EXPECT_EQ(memoryStatusRegister.AutoPowerDownEnable, 0);
    EXPECT_EQ(memoryStatusRegister.WriteProtectPin, 1);
}

TEST(MemoryControllerTestSPI, SendSPIWriteTest)
{
    uint32_t address = 0xAABB;

    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper("SPI1");
    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);

    MEM_ERROR ret = memoryControllerSpi.Write8BitWord(address, 0xFF);
    EXPECT_EQ(ret, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[5];
    uint16_t bufferLen = 5;
    ret = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(ret, MemoryErrorHandling::MEM_NO_ERROR);

    EXPECT_EQ(bufferLen, 4);
    EXPECT_EQ(buffer[0], 0x02);
    EXPECT_EQ(buffer[1], 0XAA);
    EXPECT_EQ(buffer[2], 0xbB);
    EXPECT_EQ(buffer[3], 0xFF);
}


TEST(MemoryControllerTestSPI, Write8BitWordError)
{
    uint32_t address = 0xAAAAAAAA;

    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper("SPI1");
    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);


    MEM_ERROR ret = memoryControllerSpi.Write8BitWord(address, 0xAA);
    EXPECT_EQ(ret, MemoryErrorHandling::MEM_INVALID_ADDRESS);
}

TEST(MemoryControllerTestSPI, SetWriteEnableLatch)
{
    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper(testInterfaceWrapper);

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MEM_ERROR err = memoryControllerSpi.SetWriteEnableLatch(false, 0);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[2];
    uint16_t bufferLen = 2;
    err = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(bufferLen, 1);

    EXPECT_EQ(buffer[0], 0x06);
}

TEST(MemoryControllerTestSPI, TestResetWriteEnableLatch)
{
    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper(testInterfaceWrapper);
    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MEM_ERROR err = memoryControllerSpi.Reset_WriteEnableLatch();
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[2];
    uint16_t bufferLen = 2;
    err = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(bufferLen, 1);

    EXPECT_EQ(buffer[0], 0x04);
}

TEST(MemoryControllerTestSPI, ReadStatusRegister)
{
    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper(testInterfaceWrapper);

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MemoryControllerSPI::MemoryStatusRegister statusRegister;
    MEM_ERROR err = memoryControllerSpi.ReadStatusRegister(statusRegister);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[2];
    uint16_t bufferLen = 2;
    err = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(bufferLen, 1);

//    EXPECT_EQ(buffer[0], 0x05); TODO
}

TEST(MemoryControllerTestSPI, TestResumeFromPowerDown)
{
    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper(testInterfaceWrapper);

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;

    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MEM_ERROR err = memoryControllerSpi.ResumeFromPowerDown();
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[2];
    uint16_t bufferLen = 2;
    err = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(bufferLen, 1);
    EXPECT_EQ(buffer[0], 0xAB);
}

TEST(MemoryControllerTestSPI, TestSetUltraDeepPowerDown)
{
    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper(testInterfaceWrapper);

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MEM_ERROR err = memoryControllerSpi.SetUltraDeepPowerDown();
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[2];
    uint16_t bufferLen = 2;
    err = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(bufferLen, 1);
    EXPECT_EQ(buffer[0], 0x79);
}

TEST(MemoryControllerTestSPI, TestSleepMode)
{
    TestInterfaceWrapper testInterfaceWrapper{};
    SPIWrapper spiWrapper(testInterfaceWrapper);

    RERAM_ADESTO_RM25C512C_LTAI_T memoryModule;
    MemoryControllerSPI memoryControllerSpi(&spiWrapper, memoryModule);
    MEM_ERROR err = memoryControllerSpi.SetSleepMode();
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);

    uint8_t buffer[2];
    uint16_t bufferLen = 2;
    err = testInterfaceWrapper.ReadBuffer(buffer, &bufferLen);
    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(buffer[0], 0b10111001);
}



