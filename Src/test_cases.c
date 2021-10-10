//
// Created by florianfrank on 02.07.21.
//

#include <stddef.h>
#include <stdlib.h>
#include <cmd_parser.h>
#include <time.h>
#include <usb_handler.h>
#include <meta_data_handler.h>
#include "usb_defines.h"
#include "test_cases.h"
#include "time_measurement.h"

#define NR_TESTS 100

int executeReadLatencyTestFRAMRohm()
{
    srand(time(NULL));
    uint8_t randomNumber = rand() % 256;

    //int startValue = 0x55;
    //printf("Set random number: 0x%x\n", randomNumber);

    for (int cellCtr = 0; cellCtr < MEM_SIZE_ADR; cellCtr++)
    {
        MEM_ERROR errCode = SRAM_Write_8b(0x00 + cellCtr, randomNumber);
        if(errCode != MEM_NO_ERROR)
            printf("Line: %d MEM_ERROR: %d\n", __LINE__, errCode);

        uint8_t retValue = 0x00;
        errCode = SRAM_Read_8b(0x00 + cellCtr, &retValue);
         if(errCode != MEM_NO_ERROR)
             printf("MEM_ERROR: %d\n", errCode);
        if (retValue != randomNumber)
        {
               printf("Cell: %d Line: %d Incorrect value 0x%x!=0x%x\n", cellCtr, __LINE__, retValue, randomNumber);
        }
    }

    USB_MS_Handle *usbHandle = malloc(sizeof(USB_MS_Handle));
    //USB_MS_Handle *usbHandleConfig = malloc(sizeof(USB_MS_Handle));


    USB_InitConnection(usbHandle);
    USB_ERROR ret;
    do{
        ret = USB_ExecuteStateMachine(usbHandle, 1000);
        printf("%s", USB_ReturnErrorCodeStr(ret));
    }while(ret.m_ErrCode != USB_NO_ERROR);
    printf("DONE\n");

    for(uint16_t i = 10; i < 30; i++)
    {

        char fileName[20];
        sprintf(fileName, "Meas%d.csv", i);
        printf("Write file %s\n", fileName);

        char configFile[20];
        sprintf(configFile, "Meas%d.cnf", i);

        uint8_t initvalue = 0x00;
        uint8_t valueWritten = 0x00;
        uint32_t readCycles = 1000;

        char* config = CreateMetaDataReadLatency(i, 1, initvalue, valueWritten, 0, MEM_SIZE_ADR, readCycles, 0, 3.3, 255);
        printf("TEST %s\n", config);
        uint32_t lenConfig = strlen(config);
        ret = USB_OpenWriteFile(usbHandle, configFile, (uint8_t*)config, &lenConfig, USB_APPEND | USB_WRITE, true);
        USB_CloseFile(usbHandle);



        uint32_t len = strlen("Cell;time_per_1000");
        ret = USB_OpenWriteFile(usbHandle, fileName, (uint8_t*)"Cell;time_per_1000", &len, USB_APPEND | USB_WRITE, true);


        char buffer[4096];
        memset(buffer, 0x00, 4096);

        char buff[512];
        memset(buff, 0x00, 512);
//  int buffLen = 512;
        //uint32_t start = StartTimer();
        int offset = 0;

        for (int i0 = offset; i0 < MEM_SIZE_ADR + offset; i0++) {
            SRAM_Write_8b(0x00 + i0, initvalue);

        }

        /* for(int i2 = 0; i2 < 1000; i2++)
         {
         for(int i = 0+offset; i < 64+offset; i++)
         {
         uint8_t value = 0x55;
         SRAM_Write_8b(0x00 + i, 0xAA);
         }

         }*/

//  for(int i2 = offset; i2 < (128+offset); i2++)
//  {
//	  printf("%d. Ret %d\n", i2-offset, SRAM_Read_8b(0x00 + i2));
//
//  }
        uint8_t t;
        for (int i2 = 0; i2 < MEM_SIZE_ADR; i2++) {
            uint32_t startTS = StartTimer();
            for (int i3 = 0; i3 < readCycles; i3++) {
                SRAM_Read_8b(0x00 + i2, &t);
            }
            uint32_t stop = StopGetTime();
            ResetTimer();
            sprintf(&buffer[strlen(buffer)], "%lu;%lu\n", (unsigned long) i2,
                    (unsigned long) stop - startTS);
            if (i2 % 100 == 0) {
                uint32_t size = strlen(buffer);
                USB_WriteData(usbHandle, (uint8_t*) buffer, &size, true);
                memset(buffer, 0x00, 4096);
            }
        }
        uint32_t size = strlen(buffer);
        USB_WriteData(usbHandle, (uint8_t*) buffer, &size, true);
        printf("Writing Done\n");
        USB_CloseFile(usbHandle);
        printf("Fill Buffer\n");
    }
    return 0;
}

int executeMemoryTest()
{
    for(int i = 0; i < 10000;i++)
    {

        SRAM_Write_16b(i, 0xAAAA);


        uint16_t value = 0;
        SRAM_Read_16b(i, &value);
        printf("%d Value 0x%x\n", i, value);

    }



    return 0;

}

#if MEM_ACCESS_IF == SPI
int executeWIPPollingTestAdestoReRam()
{
    srand(time(NULL));
    uint8_t randomNumber = rand() % 256;

    int startValue = 0x55;
    printf("Set random number: 0x%x\n", randomNumber);

    for (int cellCtr = 0; cellCtr < MEM_SIZE_ADR; cellCtr++)
    {
        MEM_ERROR errCode = SRAM_Write_8b(0x00 + cellCtr, randomNumber);
        if(errCode != MEM_NO_ERROR)
            printf("Line: %d MEM_ERROR: %d\n", __LINE__, errCode);

        WIP_Polling(0);

        uint8_t retValue = 0x00;
        errCode = SRAM_Read_8b(0x00 + cellCtr, &retValue);
        if(errCode != MEM_NO_ERROR)
             printf("MEM_ERROR: %d\n", errCode);
        if (retValue != randomNumber)
        {
               printf("Cell: %d Line: %d Incorrect value 0x%x!=0x%x\n", cellCtr, __LINE__, retValue, randomNumber);
        }
    }

    for (int cellCtr = 0; cellCtr < MEM_SIZE_ADR; cellCtr++)
    {
        MEM_ERROR errCode = SRAM_Write_8b(0x00 + cellCtr, startValue);
        if(errCode != MEM_NO_ERROR)
            printf("Line: %d MEM_ERROR: %d\n", __LINE__, errCode);
    }

    int MEM_ID = 1;
    int READ_ITERATIONS = 10000;
    char pollingBuffer[4096];

    memset(pollingBuffer, 0x00, 4096);


    USB_MS_Handle *usbHandle = malloc(sizeof(USB_MS_Handle));

    USB_InitConnection(usbHandle);
    USB_ERROR ret;
    do
    {
        ret = USB_ExecuteStateMachine(usbHandle, 1000);
        if (ret.m_ErrCode != USB_NO_ERROR)
        {
            printf("Line: %d %s", __LINE__, USB_ReturnErrorCodeStr(ret));
        }
    } while (ret.m_ErrCode != USB_NO_ERROR);

    for (int testCtr = 0; testCtr < NR_TESTS; testCtr++)
    {


        printf("Write Meta data %d\n", testCtr);
        char *config = CreateMetaDataReadLatency(testCtr, MEM_ID, 0xAA, 0x55, 0, MEM_SIZE_ADR, READ_ITERATIONS, 0,
                                                 3.3f, 255);
        uint32_t lenConfig = strlen(config);
        char configFileName[50];
        sprintf(configFileName, "Cfg%d.csv", testCtr);
        ret = USB_OpenWriteFile(usbHandle, configFileName, (uint8_t *) config, &lenConfig, USB_APPEND | USB_WRITE,
                                true);
        if (ret.m_ErrCode != USB_NO_ERROR)
        {
              printf("Line: %d %s", __LINE__, USB_ReturnErrorCodeStr(ret));
        }
        USB_CloseFile(usbHandle);
        usbHandle->m_Open = true; // ugly hack

        char measureFileName[50];
        sprintf(measureFileName, "Msr%d.csv", testCtr);

        char *measureFile = "Cell;WIP_Polling\n";
        uint32_t lenMeasureFile = strlen(measureFile);
        ret = USB_OpenWriteFile(usbHandle, measureFileName, (uint8_t *) measureFile, &lenMeasureFile,
                                USB_APPEND | USB_WRITE, true);
         if (ret.m_ErrCode != USB_NO_ERROR)
            printf("%s", USB_ReturnErrorCodeStr(ret));



        for (int addrCtr = 0; addrCtr < MEM_SIZE_ADR; addrCtr++)
        {
            SRAM_Write_8b(addrCtr, 0x55);

            // Wait until the WEL latch turns reset
            WIP_Polling(0);

            // Write "1010101010"
            SRAM_Write_8b(addrCtr, 0xAA);

            // Measure write latency

            int write_latency = WIP_Polling(0);

            sprintf(&pollingBuffer[strlen(pollingBuffer)], "%d;%d\n", addrCtr, write_latency);

            if (addrCtr % 100 == 0)
            {
                uint32_t lenPollingBuff = strlen(pollingBuffer);
                USB_ERROR usbError = USB_WriteData(usbHandle, (uint8_t *) pollingBuffer, &lenPollingBuff, true);
                if (usbError.m_ErrCode != USB_NO_ERROR)
                    printf("ERROR %d, Iteration %d MSG: %s", __LINE__, addrCtr, USB_ReturnErrorCodeStr(usbError));
                memset(pollingBuffer, 0x00, 4096 * sizeof(char));
            }
        }
        uint32_t lenPollingBuff = strlen(pollingBuffer);
        USB_ERROR usbError = USB_WriteData(usbHandle, (uint8_t *) pollingBuffer, &lenPollingBuff, true);
      if(usbError.m_ErrCode != USB_NO_ERROR)
          printf("ERROR %d, MSG: %s", __LINE__, USB_ReturnErrorCodeStr(usbError));
        USB_CloseFile(usbHandle);

    }
    return 0;
}
#endif // MEM_ACCESS_IF == SPI