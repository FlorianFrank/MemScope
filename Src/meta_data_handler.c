#include <argz.h>
//
// Created by florianfrank on 01.07.21.
//

#include "meta_data_handler.h"

char buffer[4096];

char* testNameStr[] = {"Write_Latency",
                      "Read_Latency",
                      "Row_Hammering",
                      "Supply_Voltage_Variation",
                      "Temperature_Dependency",
                      "Startup_Values"};

char* CreateMetaData(uint32_t testID, TestType testType, uint32_t memID, uint8_t initValue, uint8_t valueWritten,
                     uint32_t startAddr, uint32_t endAddr, uint32_t writeCycles, uint32_t readCycles,
                     uint32_t rowOffset, uint32_t columnOffset, uint32_t parallelReadBytes, uint32_t parallelWriteBytes,
                     float voltage, float temperature)
{
    // e.g. 1;Fujitsu_FRAM;9;TEST;Row_Hammering;intValue;writeCycles
    sprintf(buffer, "TEST_ID;MEM_NAME;MEM_ID;TEST_NAME;INIT_VALUE;VALUE_WRITTEN;START_ADDR;END_ADDR;WRITE_CYCLES;READ_CYCLES;ROW_OFFSET;COLUMN_OFFSET;PARALLEL_READ_BYTES;PARALLEL_WRITE_BYTES;VOLTAGE;TEMPERATURE\n");
    sprintf(&buffer[strlen(buffer)], "%d;%s;%x;%s;%x;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%f\n", (int)testID, "TEST" /*TODO*/, (int)memID, testNameStr[(int)testType], (int)initValue, (int)valueWritten, (int)startAddr, (int)endAddr, (int)writeCycles, (int)readCycles, (int)rowOffset, (int)columnOffset, (int)parallelReadBytes, (int)parallelWriteBytes, voltage, temperature);
    return buffer;
}

char* CreateMetaDataWriteLatency(uint32_t testID, uint32_t memID, uint32_t initValue, uint32_t valueWritten, uint32_t startAddr, uint32_t endAddr, uint32_t writeCycles, uint32_t writeParallel, float voltage, float temperature)
{
    return CreateMetaData(testID, Write_Latency, memID, initValue, valueWritten, startAddr, endAddr, writeCycles, 0, 0, 0, 0, writeParallel, voltage, temperature);
}

char* CreateMetaDataReadLatency(uint32_t testID, uint32_t memID, uint32_t initValue, uint32_t valueWritten, uint32_t startAddr, uint32_t endAddr, uint32_t readCycles, uint32_t readParallel, float voltage, float temperature)
{
    return CreateMetaData(testID, Read_Latency, memID , initValue, valueWritten, startAddr, endAddr, 0, readCycles, 0, 0, readParallel, 0, voltage, temperature);
}

char* CreateMetaDataRowHammering(uint32_t testID, uint32_t memID, uint32_t initValue, uint32_t valueWritten, uint32_t startAddr, uint32_t endAddr, uint32_t writeCycles, uint32_t rowOffset, uint32_t columnOffset, uint32_t parallelWrite, uint32_t parallelRead, float voltage, float temperature)
{
    return CreateMetaData(testID, Row_Hammering, memID, initValue, valueWritten, startAddr, endAddr, writeCycles, 0, rowOffset, columnOffset, parallelRead, parallelWrite, voltage, temperature);
}