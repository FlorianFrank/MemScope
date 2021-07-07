/*
 * time_measurment.h
 *
 *  Created on: Jun 9, 2021
 *      Author: florianfrank
 */

#ifndef INC_TIME_MEASUREMENT_H_
#define INC_TIME_MEASUREMENT_H_

#include <stdint.h>

uint32_t StartTimer();
void ResetTimer();
uint32_t StopGetTime();

#endif /* INC_TIME_MEASUREMENT_H_ */

