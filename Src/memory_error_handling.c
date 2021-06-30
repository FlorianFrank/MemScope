/*
 * memory_error_handling.c
 *
 *  Created on: Jun 30, 2021
 *      Author: florianfrank
 */

#ifndef SRC_MEMORY_ERROR_HANDLING_C_
#define SRC_MEMORY_ERROR_HANDLING_C_

#include "memory_error_handling.h"


#define HAL_OK       0x00U
#define HAL_ERROR    0x01U
#define HAL_BUSY     0x02U
#define HAL_TIMEOUT  0x03U

MEM_ERROR HAL_StatusTypeDefToErr(int status)
{
	switch(status)
	{
	case (HAL_OK):
			return MEM_NO_ERROR;
	case (HAL_ERROR):
			return MEM_HAL_INTERNAL_ERROR;
	case (HAL_BUSY):
			return MEM_HAL_BUSY;
	case (HAL_TIMEOUT):
		return MEM_HAL_TIMEOUT;
	default:
		return MEM_UNKNOWN_ERROR;
	}
}



#endif /* SRC_MEMORY_ERROR_HANDLING_C_ */
