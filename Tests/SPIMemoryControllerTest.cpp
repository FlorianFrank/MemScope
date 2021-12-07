#include <gtest/gtest.h>
#include "cpp/MemoryControllerWrappers/MemoryControllerSPI.h"
#include "cpp/MemoryErrorHandling.h"

TEST(MemoryControllerTestSPI, TestWriteAdesto)
{

    uint8_t retValue[4];
    uint16_t retSize = 4;

    MEM_ERROR ret = MemoryControllerSPI::CreateWriteMessageReRAMAdesto(0x55AA, 0xFB, retValue, &retSize);

    EXPECT_EQ(ret,MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(retSize, 4);
    EXPECT_EQ(retValue[0], 0x02);
    EXPECT_EQ(retValue[1], 0x55);
    EXPECT_EQ(retValue[2], 0xAA);
    EXPECT_EQ(retValue[3], 0xFB);
}

TEST(MemoryControllerTestSPI, TestInvalidArgument)
{
    uint8_t retValue[3];
    uint16_t retSize = 3;
    MEM_ERROR ret = MemoryControllerSPI::CreateWriteMessageReRAMAdesto(0x55AA, 0xFB, retValue, &retSize);
    EXPECT_EQ(ret,MemoryErrorHandling::MEM_BUFFER_TO_SMALL);

    ret = MemoryControllerSPI::CreateWriteMessageReRAMAdesto(0xAAAAAAAA, 0xFB, retValue, &retSize);
    EXPECT_EQ(ret,MemoryErrorHandling::MEM_INVALID_ADDRESS);
}

TEST(MemoryControllerTestSPI, ParseStatusRegister)
{
    uint8_t registerValue = 0x9D; // 10011101
    MemoryControllerSPI::MemoryStatusRegister memoryStatusRegister = MemoryControllerSPI::ParseStatusRegister(registerValue);

    EXPECT_EQ(memoryStatusRegister.write_in_progress_bit, 1);
    EXPECT_EQ(memoryStatusRegister.write_enable_bit, 0);
    EXPECT_EQ(memoryStatusRegister.block_protection_bits, 3);
    EXPECT_EQ(memoryStatusRegister.auto_power_down_enable, 0);
    EXPECT_EQ(memoryStatusRegister.wp_enable_Pin, 1);
}

TEST(MemoryControllerTestSPI, SendSPIWriteTest)
{
    uint32_t address = 0xAABB;


    SPIWrapper spiWrapper(0);
    MemoryControllerSPI memoryControllerSpi(&spiWrapper);

    MEM_ERROR ret = memoryControllerSpi.Write8BitWord(address, 0xFF);

    EXPECT_EQ(ret, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(spiWrapper.buffer[0], 0x02);
    EXPECT_EQ(spiWrapper.buffer[1], 0XAA);
    EXPECT_EQ(spiWrapper.buffer[2], 0xbB);
    EXPECT_EQ(spiWrapper.buffer[3], 0xFF);
}


TEST(MemoryControllerTestSPI, Write8BitWordError)
{
    uint32_t address = 0xAAAAAAAA;


    SPIWrapper spiWrapper(0);
    MemoryControllerSPI memoryControllerSpi(&spiWrapper);


    MEM_ERROR ret = memoryControllerSpi.Write8BitWord(address, 0xAA);
    EXPECT_EQ(ret, MemoryErrorHandling::MEM_INVALID_ADDRESS);
}

TEST(MemoryControllerTestSPI, SetWriteEnableLatch)
{
    SPIWrapper spiWrapper(0);
    MemoryControllerSPI memoryControllerSpi(&spiWrapper);
    MEM_ERROR err = memoryControllerSpi.Set_WriteEnableLatch(false);

    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
    EXPECT_EQ(spiWrapper.buffer[0], 0x06);
}

TEST(MemoryControllerTestSPI, ReadStatusRegister)
{
    SPIWrapper spiWrapper(0);
    MemoryControllerSPI memoryControllerSpi(&spiWrapper);
    MemoryControllerSPI::MemoryStatusRegister statusRegister;
    MEM_ERROR err = memoryControllerSpi.ReadStatusRegister(&statusRegister);

    EXPECT_EQ(err, MemoryErrorHandling::MEM_NO_ERROR);
}


