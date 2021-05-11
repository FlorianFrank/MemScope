/*
 * metrics.c
 *
 *  Created on: May 11, 2021
 *      Author: florianfrank
 */
#include "metrics.h"




/*
 * @brief								determines the number of flipped ones in a 16 bit string compared
 * 										to the expected value
 * @param uint16_t expected_value		the expected value
 * @param uint16_t real_value			the (possibly changed) value from the SRAM
 */
uint16_t flipped_one_16b(uint16_t expected_value, uint16_t real_value){
	uint16_t flipped = expected_value ^ real_value;
	flipped = expected_value & flipped;

	return hamming_weight_16b(flipped);
}

/*
 * @brief								determines the number of flipped zeros in a 16 bit string compared
 * 										to the expected value
 * @param uint16_t expected_value		the expected value
 * @param uint16_t real_value			the (possibly changed) value from the SRAM
 */
uint16_t flipped_zero_16b(uint16_t expected_value, uint16_t real_value){
	uint16_t flipped = expected_value ^ real_value;
	flipped = flipped & real_value;

	return hamming_weight_16b(flipped);
}


/*
 * @brief								determines the number of flipped ones in a 8 bit string compared
 * 										to the expected value
 * @param uint8_t expected_value		the expected value
 * @param uint8_t real_value			the (possibly changed) value from the SRAM
 */
uint8_t flipped_one_8b(uint8_t expected_value, uint8_t real_value){
	uint8_t flipped = expected_value ^ real_value;
	flipped = expected_value & flipped;

	return hamming_weight_8b(flipped);
}

/*
 * @brief								determines the number of flipped zeros in a 8 bit string compared
 * 										to the expected value
 * @param uint8_t expected_value		the expected value
 * @param uint8_t real_value			the (possibly changed) value from the SRAM
 */
uint8_t flipped_zero_8b(uint8_t expected_value, uint8_t real_value){
	uint8_t flipped = expected_value ^ real_value;
	flipped = flipped & real_value;

	return hamming_weight_8b(flipped);
}


/*
 * @brief							determines the number of ones in a 16 bit string
 * @param uint16_t bitstring		the bitstring
 */
uint16_t get_num_one_16b(uint16_t bitstring){
	return hamming_weight_16b(bitstring);
}

/*
 * @brief							determines the number of zeros in a 16 bit string
 * @param uint16_t bitstring		the bitstring
 */
uint16_t get_num_zero_16b(uint16_t bitstring){
	return (16 - hamming_weight_16b(bitstring));
}


/*
 * @brief							determines the number of ones in a 8 bit string
 * @param uint8_t bitstring		the bitstring
 */
uint8_t get_num_one_8b(uint8_t bitstring){
	return hamming_weight_8b(bitstring);
}


/*
 * @brief							determines the number of zeros in a 8 bit string
 * @param uint8_t bitstring		the bitstring
 */
uint8_t get_num_zero_8b(uint8_t bitstring){
	return (8 - hamming_weight_8b(bitstring));
}


/*
 * @brief							determines the number of ones in a 16 bit string
 * @param uint16_t bitstring		the bit string to be counted
 */
uint16_t hamming_weight_16b(uint16_t bitstring){
	uint16_t result;
	result = bitstring;
	uint16_t a = (result >> 0) & 0x5555;
	uint16_t b = (result >> 1) & 0x5555;
	result = a + b;
	a = (result >> 0) & 0x3333;
	b = (result >> 2) & 0x3333;
	result = a + b;
	a = (result >> 0) & 0x0F0F;
	b = (result >> 4) & 0x0F0F;
	result = a + b;
	a = (result >> 0) & 0x00FF;
	b = (result >> 8) & 0x00FF;
	result = a + b;

	return result;
}


/*
 * @brief							determines the number of ones in a 8 bit string
 * @param uint8_t bitstring		    the bit string to be counted
 */
uint8_t hamming_weight_8b(uint8_t bitstring){
	uint8_t result;
	result = bitstring;
	uint8_t a = (result >> 0) & 0x55;
	uint8_t b = (result >> 1) & 0x55;
	result = a + b;
	a = (result >> 0) & 0x33;
	b = (result >> 2) & 0x33;
	result = a + b;
	a = (result >> 0) & 0x0F;
	b = (result >> 4) & 0x0F;
	result = a + b;
	a = (result >> 0) & 0x0F;
	b = (result >> 8) & 0x0F;
	result = a + b;

	return result;
}
