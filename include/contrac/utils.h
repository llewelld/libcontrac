/** \ingroup Utils
 * @file
 * @author	David Llewellyn-Jones <david@flypig.co.uk>
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2020
 * Released under the GPLv2.
 *
 * @brief Static utility functions
 * @section DESCRIPTION
 *
 * Provides various static utitlity functions. In particular:
 *
 * base64 encoding and decoding functionality.
 * Time conversion: from epoch to day numbers and time interval numbers.
 *
 */

/** \addtogroup Utils
 *  @{
 */

#ifndef __UTILS_H
#define __UTILS_H

// Includes

#include <time.h>

// Defines

#define MAX(a,b) \
	({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a > _b ? _a : _b; })

#define MIN(a,b) \
	({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a < _b ? _a : _b; })

// Structures

// Function prototypes

size_t base64_encode_size(size_t binary_input);
size_t base64_decode_size(size_t base64_input);
void base64_encode_binary_to_base64(unsigned char const *input, size_t input_size, unsigned char *output, size_t *output_size);
void base64_decode_base64_to_binary(unsigned char const *input, size_t input_size, unsigned char *output, size_t *output_size);

uint32_t epoch_to_day_number(time_t epoch);
uint8_t epoch_to_time_interval_number(time_t epoch);

// Function definitions

#endif // __UTILS_H

/** @} addtogroup Utils */

