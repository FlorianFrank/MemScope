//
// Created by florianfrank on 08.12.21.
//

#ifndef MEMORY_TESTING_FW_BASIC_DEFINES_H
#define MEMORY_TESTING_FW_BASIC_DEFINES_H

typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;


#endif //MEMORY_TESTING_FW_BASIC_DEFINES_H
