#pragma once

#include <cstdint>

#define MAX_RESPONSE_SIZE 128

typedef enum JSON_PARSER_STATUS {
    JSON_RECEIVED_CMD,
    JSON_PROCESSING,
    JSON_READY,
    JSON_ERROR,
} JSON_PARSER_STATUS;

// Enum for data types.
typedef enum DataType {
    INTEGER,
    BOOLEAN,
    STRING
} DataType;

// Enum for PUF types.
typedef enum PUFType {
    RELIABLE = 0,
    WRITE_LATENCY = 1,
    READ_LATENCY = 2,
    ROW_HAMMERING = 3,
    UNKNOWN_PUF_TEST = 4
} PUFType;


typedef enum Cmd {
    CMD_IDN = 0x00,

    CMD_START_MEASUREMENT = 0x01,
    CMD_STOP_MEASUREMENT = 0x02,
    CMD_RETRIEVE_STATUS = 0x03,
    CMD_RESET = 0x04,
    CMD_UNDEFINED = 0x05
} Cmd;

typedef struct Response {
    Cmd command;
    JSON_PARSER_STATUS status;
    char responseMsg[MAX_RESPONSE_SIZE];
} Response;

// Structure for general configuration.
typedef struct GeneralConfig {
    Cmd command;
    PUFType pufType;
    uint32_t initValue;
    uint32_t pufValue;
    uint32_t startAddress;
    uint32_t endAddress;
    uint16_t tWaitAfterInit;
    uint16_t tNextRead;
    bool ceDrivenWrite;
    bool ceDrivenRead;
} GeneralConfig;

// Structure for write timing configuration.
typedef struct WriteTimingConfig {
    uint16_t tStart;
    uint16_t tNext;
    uint16_t tAC;
    uint16_t tAS;
    uint16_t tAH;
    uint16_t tPWD;
    uint16_t tDS;
    uint16_t tDH;
} WriteTimingConfig;

// Structure for read timing configuration.
typedef struct ReadTimingConfig {
    uint16_t tStart;
    uint16_t tAS;
    uint16_t tAH;
    uint16_t tEOD;
    uint16_t tPRC;
    uint16_t tCEOEEnable;
    uint16_t tCEOEDisable;
} ReadTimingConfig;

// Structure for row hammering configuration.
typedef struct RowHammeringConfig {
    uint16_t tWaitBetweenHammering;
    uint16_t hammeringIterations;
    uint16_t hammeringDistance;
}RowHammeringConfig;

// Enum for token types.
typedef enum TokenType {
    COMMAND,
    PUF_TYPE,
    INIT_VALUE,
    PUF_VALUE,
    START_ADDR,
    STOP_ADDR,
    CE_DRIVEN,

    T_WAIT_AFTER_INIT,
    T_NEXT_READ,
    T_START_DEFAULT,
    T_NEXT_WRITE_DEFAULT,
    T_AC_DEFAULT,
    T_AS_DEFAULT,
    T_AH_DEFAULT,
    T_PWD_DEFAULT,
    T_DS_DEFAULT,
    T_DH_DEFAULT,

    T_START_ADJUSTED,
    T_NEXT_WRITE_ADJUSTED,
    T_AC_ADJUSTED,
    T_AS_ADJUSTED,
    T_AH_ADJUSTED,
    T_PWD_ADJUSTED,
    T_DS_ADJUSTED,
    T_DH_ADJUSTED,

    CE_DRIVEN_READ,
    T_START_READ_DEFAULT,
    T_AS_READ_DEFAULT,
    T_AH_READ_DEFAULT,
    T_OED_DEFAULT,
    T_PRC_DEFAULT,
    T_CE_OE_ENABLE_DEFAULT,
    T_CE_OE_DISABLE_DEFAULT,

    T_START_READ_ADJUSTED,
    T_AS_READ_ADJUSTED,
    T_AH_READ_ADJUSTED,
    T_OED_ADJUSTED,
    T_PRC_ADJUSTED,
    T_CE_OE_ENABLE_ADJUSTED,
    T_CE_OE_DISABLE_ADJUSTED,

    T_WAIT_BETWEEN_HAMMERING,
    HAMMERING_ITERATIONS,
    HAMMERING_DISTANCE,
} TokenType;


// Structure for tokens.
typedef struct Token {
    TokenType tokenIDN;
    const char *token;
    DataType dataType;
} Token;

typedef struct PUFConfiguration {
    GeneralConfig generalConfig;
    WriteTimingConfig writeTimingConfigDefault;
    WriteTimingConfig writeTimingConfigAdjusted;
    ReadTimingConfig readTimingConfigDefault;
    ReadTimingConfig readTimingConfigAdjusted;
    RowHammeringConfig rowHammeringConfig;
} PUFConfiguration;

const char * statusToString(JSON_PARSER_STATUS status);
const char * cmdToStr(Cmd cmd);
Cmd parseCmd(char *str);
JSON_PARSER_STATUS parse_json(const char *inputBuffer, PUFConfiguration* config);