#include "cpp/JSONParser.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

const char* cmdStrList[] = {"idn", "start_measurement", "stop_measurement", "status", "reset", "undefined"};
const char* statusStrList[] = {"ok", "processing", "ready", "error"};

void remove_double_quotes(const char* input, char* output) {
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] != '\"' && input[i] != ',') { // Skip double quotes
            output[j++] = input[i];
        }
    }
    output[j] = '\0'; // Null-terminate the output string
}

// Utility function to remove spaces from a string.
void remove_spaces(char *s) {
    char *d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

// Function to find substrings between specified beginning and ending markers.
char *findBeginningEnd(char *retBegin, char *searchStr, size_t *nrBytes, char *offset, const char *beginning,
                       const char *end) {
    char *begin1 = strstr((char *) ((long) searchStr + (long) offset), beginning);
    if (!begin1) {
        //printf("Error while parsing token " << beginning << std::endl;
        *nrBytes = 0;
        return nullptr;
    }
    char *begin2 = begin1 + strlen(beginning) + 2;
    if (!begin2) {
        //printf("Error while parsing token " << beginning << std::endl;
        *nrBytes = 0;
        return nullptr;
    }
    char *end1 = strstr(begin2, end);

    if (!end1) {
        //printf("Error while parsing token " << beginning << std::endl;
        *nrBytes = 0;
        return nullptr;
    }

    memcpy(retBegin, begin2, end1 - begin2);
    retBegin[end1 - begin2] = '\0';
    remove_spaces(retBegin);
    return end1;
}


// Parses a boolean value from a string.
bool parseBoolean(char *value) {
    return strstr(value, "true") != nullptr;
}

// Parses an integer value from a string.
uint32_t parseInteger(char *value) {
    return atoi(value);
}

// Parses a PUF type from a string.
PUFType parsePUFType(char *str) {
    if (strstr(str, "reliable")) {
        return RELIABLE;
    }
    if (strstr(str, "rowHammering")) {
        return ROW_HAMMERING;
    }
    if (strstr(str, "writeLatency")) {
        return WRITE_LATENCY;
    }
    if (strstr(str, "readLatency")) {
        return READ_LATENCY;
    }
    if (strstr(str, "voltageRead")) {
        return VOLTAGE_VARIATIONS;
    }
    if (strstr(str, "voltageWrite")) {
        return VOLTAGE_VARIATIONS;
    }
    return UNKNOWN_PUF_TEST;
}

void
printConfigs(PUFConfiguration *pufConfig) {
    const char *pufTypeStr = (pufConfig->generalConfig.pufType == RELIABLE) ? "RELIABLE" :
                             (pufConfig->generalConfig.pufType == ROW_HAMMERING) ? "ROW HAMMERING" :
                             (pufConfig->generalConfig.pufType == WRITE_LATENCY) ? "WRITE_LATENCY" :
                             (pufConfig->generalConfig.pufType == READ_LATENCY) ? "READ_LATENCY" :
                             (pufConfig->generalConfig.pufType == VOLTAGE_VARIATIONS) ? "VOLTAGE_VARIATIONS" :
                             "Unknown_PUF_TEST";

    printf("\n  Command: %s\n", cmdStrList[pufConfig->generalConfig.command]);

    printf("  General Config: \n");
    printf("    PUF Type: %s\n", pufTypeStr);
    printf("    Init Value: 0x%x\n", pufConfig->generalConfig.initValue);
    printf("    PUF Value: 0x%x\n", pufConfig->generalConfig.pufValue);
    printf("    Start Address: 0x%x\n", pufConfig->generalConfig.startAddress);
    printf("    End Address: 0x%x\n", pufConfig->generalConfig.endAddress);
    printf("    Wait After Init: 0x%x\n", pufConfig->generalConfig.tWaitAfterInit);
    printf("    Next Read: 0x%x\n", pufConfig->generalConfig.tNextRead);
    printf("    CE Driven Write: %s\n", pufConfig->generalConfig.ceDrivenWrite ? "true" : "false");
    printf("    CE Driven Read: %s\n\n", pufConfig->generalConfig.ceDrivenRead ? "true" : "false");

    printf("  Default Timing Config: \n");
    printf("    Write: \n");
    printf("      Start: 0x%x\n", pufConfig->writeTimingConfigDefault.tStart);
    printf("      Next Write Default: 0x%x\n", pufConfig->writeTimingConfigDefault.tNext);
    printf("      AC: 0x%x\n", pufConfig->writeTimingConfigDefault.tAC);
    printf("      AS: 0x%x\n", pufConfig->writeTimingConfigDefault.tAS);
    printf("      AH: 0x%x\n", pufConfig->writeTimingConfigDefault.tAH);
    printf("      PWD: 0x%x\n", pufConfig->writeTimingConfigDefault.tPWD);
    printf("      DS: 0x%x\n", pufConfig->writeTimingConfigDefault.tDS);
    printf("      DH: 0x%x\n\n", pufConfig->writeTimingConfigDefault.tDH);

    printf("    Read: \n");
    printf("      Start: 0x%x\n", pufConfig->readTimingConfigDefault.tStart);
    printf("      AS: 0x%x\n", pufConfig->readTimingConfigDefault.tAS);
    printf("      AH: 0x%x\n", pufConfig->readTimingConfigDefault.tAH);
    printf("      EOD: 0x%x\n", pufConfig->readTimingConfigDefault.tEOD);
    printf("      PRC: 0x%x\n", pufConfig->readTimingConfigDefault.tPRC);
    printf("      CE/OE Enable: 0x%x\n", pufConfig->readTimingConfigDefault.tCEOEEnable);
    printf("      CE/OE Disable: 0x%x\n\n", pufConfig->readTimingConfigDefault.tCEOEDisable);

    printf("  Modified Timing Config: \n");
    printf("    Write: \n");
    printf("      Start: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tStart);
    printf("      Next Write Default: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tNext);
    printf("      AC: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tAC);
    printf("      AS: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tAS);
    printf("      AH: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tAH);
    printf("      PWD: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tPWD);
    printf("      DS: 0x%x\n", pufConfig->writeTimingConfigAdjusted.tDS);
    printf("      DH: 0x%x\n\n", pufConfig->writeTimingConfigAdjusted.tDH);

    printf("    Read: \n");
    printf("      Start: 0x%x\n", pufConfig->readTimingConfigAdjusted.tStart);
    printf("      AS: 0x%x\n", pufConfig->readTimingConfigAdjusted.tAS);
    printf("      AH: 0x%x\n", pufConfig->readTimingConfigAdjusted.tAH);
    printf("      EOD: 0x%x\n", pufConfig->readTimingConfigAdjusted.tEOD);
    printf("      PRC: 0x%x\n", pufConfig->readTimingConfigAdjusted.tPRC);
    printf("      CE/OE Enable: 0x%x\n", pufConfig->readTimingConfigAdjusted.tCEOEEnable);
    printf("      CE/OE Disable: 0x%x\n\n", pufConfig->readTimingConfigAdjusted.tCEOEDisable);

    printf("  Row Hammering Config: \n");
    printf("    Wait Between Hammering: %x\n", pufConfig->rowHammeringConfig.tWaitBetweenHammering);
    printf("    Hammering Iterations: %x\n", pufConfig->rowHammeringConfig.hammeringIterations);
    printf("    Hammering Distance: %x\n\n", pufConfig->rowHammeringConfig.hammeringDistance);
}

const char * cmdToStr(Cmd cmd) {
    return cmdStrList[cmd];
}

const char * statusToString(JSON_PARSER_STATUS status) {
    return statusStrList[status];
}

Cmd parseCmd(char *str) {
    char outStr[64];
    remove_double_quotes(str, outStr);
    printf("PARSE COMMAND: %s %s\n", cmdStrList[CMD_START_MEASUREMENT], outStr);
    if (strcmp(outStr, "idn") == 0) {
        return CMD_IDN;
    }
    if (strcmp(outStr, "start_measurement") == 0) {
        printf("Start measurement\n");
        return CMD_START_MEASUREMENT;
    }
    if (strcmp(outStr, "stop_measurement") == 0) {
        return CMD_STOP_MEASUREMENT;
    }
    if (strcmp(outStr, "status") == 0) {
        return CMD_RETRIEVE_STATUS;
    }
    if (strcmp(outStr, "reset") == 0) {
        return CMD_RESET;
    }
    return CMD_UNDEFINED;
}

// Parses JSON data.
JSON_PARSER_STATUS parse_json(const char *inputBuffer, PUFConfiguration *config) {
    char subStr[128];
    size_t nrBytes;

    Token tokens[] = {
            {COMMAND,                   "cmd",                  STRING},
            {PUF_TYPE,                 "puf_type",              STRING},
            {INIT_VALUE,               "init_value",            INTEGER},
            {PUF_VALUE,                "puf_value",             INTEGER},
            {START_ADDR,               "start_addr",            INTEGER},
            {STOP_ADDR,                "stop_addr",             INTEGER},
            {CE_DRIVEN,                "ce_driven",             BOOLEAN},

            {T_WAIT_AFTER_INIT,        "tWaitAfterInit",        INTEGER},
            {T_NEXT_READ,              "tNextRead",             INTEGER},
            {T_START_DEFAULT,          "tStartDefault",         INTEGER},
            {T_NEXT_WRITE_DEFAULT,     "tNextWriteDefault",     INTEGER},
            {T_AC_DEFAULT,             "tACDefault",            INTEGER},
            {T_AS_DEFAULT,             "tASDefault",            INTEGER},
            {T_AH_DEFAULT,             "tAHDefault",            INTEGER},
            {T_PWD_DEFAULT,            "tPWDDefault",           INTEGER},
            {T_DS_DEFAULT,             "tDSDefault",            INTEGER},
            {T_DH_DEFAULT,             "tDHDefault",            INTEGER},


            {T_START_ADJUSTED,         "tStartAdjusted",        INTEGER},
            {T_NEXT_WRITE_ADJUSTED,    "tNextWriteAdjusted",    INTEGER},
            {T_AC_ADJUSTED,            "tACAdjusted",           INTEGER},
            {T_AS_ADJUSTED,            "tASAdjusted",           INTEGER},
            {T_AH_ADJUSTED,            "tAHAdjusted",           INTEGER},
            {T_PWD_ADJUSTED,           "tPWDAdjusted",          INTEGER},
            {T_DS_ADJUSTED,            "tDSAdjusted",           INTEGER},
            {T_DH_ADJUSTED,            "tDHAdjusted",           INTEGER},
            {CE_DRIVEN_READ,           "ceDrivenRead",          BOOLEAN},

            {T_START_READ_DEFAULT,     "tStartReadDefault",     INTEGER},
            {T_AS_READ_DEFAULT,        "tASReadDefault",        INTEGER},
            {T_AH_READ_DEFAULT,        "tAHReadDefault",        INTEGER},
            {T_OED_DEFAULT,            "tOEDDefault",           INTEGER},
            {T_PRC_DEFAULT,            "tPRCDefault",           INTEGER},
            {T_CE_OE_ENABLE_DEFAULT,   "tCEOEEnableDefault",    INTEGER},
            {T_CE_OE_DISABLE_DEFAULT,  "tCEOEDisableDefault",   INTEGER},

            {T_START_READ_ADJUSTED,    "tStartReadAdjusted",    INTEGER},
            {T_AS_READ_ADJUSTED,       "tASReadAdjusted",       INTEGER},
            {T_AH_READ_ADJUSTED,       "tAHReadAdjusted",       INTEGER},
            {T_OED_ADJUSTED,           "tOEDAdjusted",          INTEGER},
            {T_PRC_ADJUSTED,           "tPRCAdjusted",          INTEGER},
            {T_CE_OE_ENABLE_ADJUSTED,  "tCEOEEnableAdjusted",   INTEGER},
            {T_CE_OE_DISABLE_ADJUSTED, "tCEOEDisableAdjusted",  INTEGER},

            {T_WAIT_BETWEEN_HAMMERING, "tWaitBetweenHammering", INTEGER},
            {HAMMERING_ITERATIONS,     "hammeringIterations",   INTEGER},
            {HAMMERING_DISTANCE,       "hammeringDistance",     INTEGER}
    };

    int tokenCtr = 0;
    for (auto & token : tokens) {
        memset(subStr, 0, 128);
        findBeginningEnd(subStr, (char*)inputBuffer, &nrBytes, nullptr, token.token, "\n");
        tokenCtr += 1;
        switch (token.tokenIDN) {
            case COMMAND:
                config->generalConfig.command = parseCmd(subStr);
                break;
            case PUF_TYPE:
                config->generalConfig.pufType = parsePUFType(subStr);
                break;
            case INIT_VALUE:
                config->generalConfig.initValue = parseInteger(subStr);
                break;
            case PUF_VALUE:
                config->generalConfig.pufValue = parseInteger(subStr);
                break;
            case START_ADDR:
                config->generalConfig.startAddress = parseInteger(subStr);
                break;
            case STOP_ADDR:
                config->generalConfig.endAddress = parseInteger(subStr);
                break;
            case CE_DRIVEN:
                config->generalConfig.ceDrivenWrite = parseBoolean(subStr);
                break;
            case T_WAIT_AFTER_INIT:
                config->generalConfig.tWaitAfterInit = (uint16_t)(parseInteger(subStr));
                break;
            case T_NEXT_READ:
                config->generalConfig.tNextRead = (uint16_t)(parseInteger(subStr));
                break;

            case T_START_DEFAULT:
                config->writeTimingConfigDefault.tStart = (uint16_t)(parseInteger(subStr));
                break;
            case T_NEXT_WRITE_DEFAULT:
                config->writeTimingConfigDefault.tNext = (uint16_t)(parseInteger(subStr));
                break;
            case T_AC_DEFAULT:
                config->writeTimingConfigDefault.tAC = (uint16_t)(parseInteger(subStr));
                break;
            case T_AS_DEFAULT:
                config->writeTimingConfigDefault.tAS = (uint16_t)(parseInteger(subStr));
                break;
            case T_AH_DEFAULT:
                config->writeTimingConfigDefault.tAH = (uint16_t)(parseInteger(subStr));
                break;
            case T_PWD_DEFAULT:
                config->writeTimingConfigDefault.tPWD = (uint16_t)(parseInteger(subStr));
                break;
            case T_DS_DEFAULT:
                config->writeTimingConfigDefault.tDS = (uint16_t)(parseInteger(subStr));
                break;
            case T_DH_DEFAULT:
                config->writeTimingConfigDefault.tDH = (uint16_t)(parseInteger(subStr));
                break;

            case T_WAIT_BETWEEN_HAMMERING:
                config->rowHammeringConfig.tWaitBetweenHammering = (uint16_t)(parseInteger(subStr));
                break;
            case HAMMERING_ITERATIONS:
                config->rowHammeringConfig.hammeringIterations = (uint16_t)(parseInteger(subStr));
                break;
            case HAMMERING_DISTANCE:
                config->rowHammeringConfig.hammeringDistance = (uint16_t)(parseInteger(subStr));
                break;

            default:
                printf("Unrecognized token type: %u\n",  token.tokenIDN);
                break;
            case T_START_ADJUSTED:
                config->writeTimingConfigAdjusted.tStart = (uint16_t)(parseInteger(subStr));
                break;
            case T_NEXT_WRITE_ADJUSTED:
                config->writeTimingConfigAdjusted.tNext = (uint16_t)(parseInteger(subStr));
                break;
            case T_AC_ADJUSTED:
                config->writeTimingConfigAdjusted.tAC = (uint16_t)(parseInteger(subStr));
                break;
            case T_AS_ADJUSTED:
                config->writeTimingConfigAdjusted.tAS = (uint16_t)(parseInteger(subStr));
                break;
            case T_AH_ADJUSTED:
                config->writeTimingConfigAdjusted.tAH = (uint16_t)(parseInteger(subStr));
                break;
            case T_PWD_ADJUSTED:
                config->writeTimingConfigAdjusted.tPWD = (uint16_t)(parseInteger(subStr));
                break;
            case T_DS_ADJUSTED:
                config->writeTimingConfigAdjusted.tDS = (uint16_t)(parseInteger(subStr));
                break;
            case T_DH_ADJUSTED:
                config->writeTimingConfigAdjusted.tDH = (uint16_t)(parseInteger(subStr));
                break;
            case CE_DRIVEN_READ:
                config->generalConfig.ceDrivenRead = (uint16_t)(parseInteger(subStr));
                break;
            case T_START_READ_DEFAULT:
                config->readTimingConfigDefault.tStart = (uint16_t)(parseInteger(subStr));
                break;
            case T_AS_READ_DEFAULT:
                config->readTimingConfigDefault.tAS = (uint16_t)(parseInteger(subStr));
                break;
            case T_OED_DEFAULT:
                config->readTimingConfigDefault.tEOD = (uint16_t)(parseInteger(subStr));
                break;
            case T_PRC_DEFAULT:
                config->readTimingConfigDefault.tPRC = (uint16_t)(parseInteger(subStr));
                break;
            case T_CE_OE_ENABLE_DEFAULT:
                config->readTimingConfigDefault.tCEOEEnable = (uint16_t)(parseInteger(subStr));
                break;
            case T_CE_OE_DISABLE_DEFAULT:
                config->readTimingConfigDefault.tCEOEDisable = (uint16_t)(parseInteger(subStr));
                break;
            case T_START_READ_ADJUSTED:
                config->readTimingConfigAdjusted.tStart = (uint16_t)(parseInteger(subStr));
                break;
            case T_AS_READ_ADJUSTED:
                config->readTimingConfigAdjusted.tAS = (uint16_t)(parseInteger(subStr));
                break;
            case T_OED_ADJUSTED:
                config->readTimingConfigAdjusted.tEOD = (uint16_t)(parseInteger(subStr));
                break;
            case T_PRC_ADJUSTED:
                config->readTimingConfigAdjusted.tPRC = (uint16_t)(parseInteger(subStr));
                break;
            case T_CE_OE_ENABLE_ADJUSTED:
                config->readTimingConfigAdjusted.tCEOEEnable = (uint16_t)(parseInteger(subStr));
                break;
            case T_CE_OE_DISABLE_ADJUSTED:
                config->readTimingConfigAdjusted.tCEOEDisable = (uint16_t)(parseInteger(subStr));
                break;
            case T_AH_READ_DEFAULT:
                config->readTimingConfigDefault.tAH = (uint16_t)(parseInteger(subStr));
                break;
            case T_AH_READ_ADJUSTED:
                config->readTimingConfigAdjusted.tAH = (uint16_t)(parseInteger(subStr));
                break;
        }
    }
    printf("Resulting parsed config:");
    printConfigs(config);
    return JSON_PARSER_STATUS::JSON_RECEIVED_CMD;
}