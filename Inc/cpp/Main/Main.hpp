/**
 * @author Florian Frank
 * @copyright University of Passau - Chair of computer engineering
 */
#ifndef MAIN_H
#define MAIN_H
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
 defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
#include "stm32f4xx_ll_fmc.h"
#endif
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#ifdef __cplusplus
extern "C" {
#endif
#define STM32_UUID ((uint32_t *)0x1FFF7A10)
extern int main();
// Reading STM32F429 unique device ID, The UUID is stored in memory at address 0x1FFF7A10
uint32_t idPart1 = STM32_UUID[0];
uint32_t idPart2 = STM32_UUID[1];
uint32_t idPart3 = STM32_UUID[2];

// Convert idPartX variables to string
std::string idPart1Str = std::to_string(idPart1);
std::string idPart2Str = std::to_string(idPart2);
std::string idPart3Str = std::to_string(idPart3);

// Concatenate idPartXStr variables to form the UUID
std::string UUID = idPart1Str + "-" + idPart2Str + "-" + idPart3Str;


extern string idn_payload = "{\"name\":\"STM32F429-Discovery\", \"serial\":\"670FF353138524E43084854\", \"owner\":\"elkhayari\", \"device_label\":\"stm-board-2\", \"externalMemory\":\"FRAM R2\" , \"UUID\":\"" + UUID + "\"}";

extern string WRITING_STATE = "WRITING";
extern string END_WRITING_STATE = "END WRITING";
extern string READING_STATE = "READING";
extern string END_READING_STATE = "END READING";
extern string FINISH = "FINISH";

#ifdef __cplusplus
}
#endif


#endif /* MAIN_H */