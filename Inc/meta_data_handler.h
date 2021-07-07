#include <argz.h>
//
// Created by florianfrank on 01.07.21.
//

#ifndef MEMORY_TESTING_FW_META_DATA_HANDLER_H
#define MEMORY_TESTING_FW_META_DATA_HANDLER_H

#include "memory_defines.h"
#include <stdint.h> // uint32_t
#include <string.h> // strlen
#include <stdio.h> // sprintf

enum {
    Write_Latency,
    Read_Latency,
    Row_Hammering,
    Supply_Volage_Variation __unused,
    Temperature_Dependency __unused,
    Startup_Values __unused
} typedef TestType;


char buffer[4096];

char* CreateMetaData(uint32_t testID, TestType testType, uint32_t memID, uint8_t initValue, uint8_t valueWritten,
                     uint32_t startAddr, uint32_t endAddr, uint32_t writeCycles, uint32_t readCycles,
                     uint32_t rowOffset, uint32_t columnOffset, uint32_t parallelReadBytes, uint32_t parallelWriteBytes,
                     float voltage, float temperature);

__unused char* CreateMetaDataWriteLatency(uint32_t testID, uint32_t memID, uint32_t initValue, uint32_t valueWritten, uint32_t startAddr, uint32_t endAddr, uint32_t writeCycles, uint32_t writeParallel, float voltage, float temperature);

char* CreateMetaDataReadLatency(uint32_t testID, uint32_t memID, uint32_t initValue, uint32_t valueWritten, uint32_t startAddr, uint32_t endAddr, uint32_t readCycles, uint32_t readParallel, float voltage, float temperature);

__unused char* CreateMetaDataRowHammering(uint32_t testID, uint32_t memID, uint32_t initValue, uint32_t valueWritten, uint32_t startAddr, uint32_t endAddr, uint32_t writeCycles, uint32_t rowOffset, uint32_t columnOffset, uint32_t parallelWrite, uint32_t parallelRead, float voltage, float temperature);


#endif //MEMORY_TESTING_FW_META_DATA_HANDLER_H
