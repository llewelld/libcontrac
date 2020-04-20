/** \ingroup contrac
 * @file
 * @author	David Llewellyn-Jones
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 *
 *
 * @brief
 * @section DESCRIPTION
 *
 *
 *
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





