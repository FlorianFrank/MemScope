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
extern string idn_payload = "{\"name\":\"STM32F429-Discovery\", \"serial\":\"670FF353138524E43084854\", \"owner\":\"elkhayari\", \"externalMemory\":\"FRAM R2\"}";

extern string WRITING_STATE = "WRITING";
extern string END_WRITING_STATE = "END WRITING";
extern string READING_STATE = "READING";
extern string END_READING_STATE = "END READING";
extern string FINISH = "FINISH";

#ifdef __cplusplus
}
#endif


#endif /* MAIN_H */