/*
 * time_measurment.h
 *
 *  Created on: Jun 9, 2021
 *      Author: florianfrank
 */

#ifndef INC_TIME_MEASUREMENT_H_
#define INC_TIME_MEASUREMENT_H_

#include <stdint.h>

void USB_StartTimer();
void USB_ResetTimer();
uint32_t USB_StopGetTime();

#endif /* INC_TIME_MEASUREMENT_H_ */

