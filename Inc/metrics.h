/*
 * metrics.h
 *
 *  Created on: May 11, 2021
 *      Author: florianfrank
 */

#ifndef INC_METRICS_H_
#define INC_METRICS_H_

#include <stdint.h>
#include <stdlib.h>

uint16_t hamming_weight_16b(uint16_t bitstring);
uint8_t hamming_weight_8b(uint8_t bitstring);

uint16_t get_num_one_16b(uint16_t bitstring);
uint16_t get_num_zero_16b(uint16_t bitstring);
uint8_t get_num_one_8b(uint8_t bitstring);
uint8_t get_num_zero_8b(uint8_t bitstring);
uint16_t flipped_one_16b(uint16_t expected_value, uint16_t real_value);
uint16_t flipped_zero_16b(uint16_t expected_value, uint16_t real_value);
uint8_t flipped_one_8b(uint8_t expected_value, uint8_t real_value);
uint8_t flipped_zero_8b(uint8_t expected_value, uint8_t real_value);

#endif /* INC_METRICS_H_ */
