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

#define ADDRESSES_TO_TEST MEM_SIZE_ADR
#define READ_ITERATIONS 10000
#define MEM_ID 4

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
    USB_MS_Handle *usbHandle = calloc(1, sizeof(USB_MS_Handle));

    USB_InitConnection(usbHandle);
    USB_ERROR ret;
    do {
        ret = USB_ExecuteStateMachine(usbHandle, 1000);
        if (ret.m_ErrCode != USB_NO_ERROR) {
            printf("Line: %d %s", __LINE__, USB_ReturnErrorCodeStr(ret));
        }
    } while (ret.m_ErrCode != USB_NO_ERROR);

    char *pollingBuffer = calloc(4096, sizeof(char));

    char *measureFileName = calloc(128, sizeof(char));
    sprintf(measureFileName, "Measurements%d.csv", MEM_ID);
    char *measureFile = "From;To;Cell;WIP_Polling\n";
    uint32_t lenMeasureFile = strlen(measureFile);
    ret = USB_OpenWriteFile(usbHandle, measureFileName, (uint8_t *) measureFile, &lenMeasureFile,
                            USB_APPEND | USB_WRITE, true);
    if (ret.m_ErrCode != USB_NO_ERROR)
        printf("%s", USB_ReturnErrorCodeStr(ret));

    uint8_t num1 = 0, num2 = 0;
    do {
        do {
            printf("Numbers: 0x%02x -> 0x%02x\n", num1, num2);
            for (int addrCtr = 0; addrCtr < ADDRESSES_TO_TEST; addrCtr++) {
                // Write first value
                SRAM_Write_8b(addrCtr, num1);

                // Wait until the WEL latch turns reset
                WIP_Polling(0);

                // Overwrite value
                SRAM_Write_8b(addrCtr, num2);

                // Measure write latency
                int write_latency = WIP_Polling(0);

                sprintf(&pollingBuffer[strlen(pollingBuffer)], "%d;%d;%d;%d\n", num1, num2, addrCtr, write_latency);

                if (addrCtr % 100 == 0 || addrCtr >= ADDRESSES_TO_TEST - 1) {
                    uint32_t lenPollingBuff = strlen(pollingBuffer);
                    USB_ERROR usbError = USB_WriteData(usbHandle, (uint8_t *) pollingBuffer, &lenPollingBuff, true);
                    if (usbError.m_ErrCode != USB_NO_ERROR)
                    printf("ERROR %d, Iteration %d MSG: %s", __LINE__, addrCtr, USB_ReturnErrorCodeStr(usbError));
                    memset(pollingBuffer, 0x00, 4096 * sizeof(char));
                }
            }
        } while (num2++ != 255);
        USB_ERROR usbError = USB_Flush(usbHandle);
        if (usbError.m_ErrCode != USB_NO_ERROR)
            printf("ERROR %d, MSG: %s", __LINE__, USB_ReturnErrorCodeStr(usbError));
    } while (num1++ != 255);

    USB_CloseFile(usbHandle);
    free(measureFileName);
    free(pollingBuffer);
    return 0;
}
#endif // MEM_ACCESS_IF == SPI