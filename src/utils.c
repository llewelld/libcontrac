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
 * @brief Static utitlity functions. 
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

// Includes

#include <openssl/evp.h>

// Defines

// Structures

// Function prototypes

// Function definitions

// Function definitions

/**
 * Returns the amount of space needed to store the base64 equivalent of a binary
 * input.
 *
 * When converting to base64 it's often useful to know how much space will be
 * needed to store the result, for example so that a buffer of the correct size
 * can be allocated for it.
 *
 * This function returns the size needed for a buffer that will be large enough
 * to store the result, including a terminating null character. The returned
 * value may be larger than the size actually needed.
 *
 * @param binary_input The length of binary input that would be encoded.
 * @return The buffer size needed to store the base64 version of the same data.
 */
size_t base64_encode_size(size_t binary_input) {
	return (((size_t)((binary_input + 2) / 3)) * 4) + 1;
}

/**
 * Returns the amount of space needed to store the binary equivalent of a base64
 * string.
 *
 * When converting from base64 it's often useful to know how much space will be
 * needed to store the result, for example so that a buffer of the correct size
 * can be allocated for it.
 *
 * This function returns the size needed for a buffer that will be large enough
 * to store the result. The returned value may be larger than the size actually
 * needed.
 *
 * @param base64_input The length of a base64 string that would be decoded.
 * @return The buffer size needed to store the binary version of the same data.
 */
size_t base64_decode_size(size_t base64_input) {
	return (((size_t)((base64_input + 3) / 4)) * 3) + 1;
}

/**
 * Encodes binary data to base64 format string.
 *
 * Encodes the binary data provided into a base64 string, storing the result
 * in the output buffer provided. The output buffer must be pre-allocated
 * with enough space to store the result. The size needed can be found by
 * calling the \ref base64_encode_size() function.
 *
 * If the output buffer is too small (based on the size provided) then the
 * base64 string may be only partially written.
 *
 * @param input The binary data to encode. This doesn't need to be zero
 *        terminated.
 * @param input_size The size of the input buffer to be converted.
 * @param output A pre-allocated buffer to store the result.
 * @param output_size The size of the allocated buffer, which will be updated
 *        to the number of bytes written to the buffer.
 */
void base64_encode_binary_to_base64(unsigned char const *input, size_t input_size, unsigned char *output, size_t *output_size) {
	size_t size_in;
	size_t size_out;

	size_in = input_size;
	size_out = base64_encode_size(input_size);
	
	if (size_out > *output_size) {
		size_in = base64_decode_size(*output_size - 1) - 1;
	}
	*output_size = base64_encode_size(size_in);

	EVP_EncodeBlock(output, input, size_in);
}

/**
 * Decodes a base64 string into the original binary data it represents.
 *
 * Decodes the base64 string provided into binary, storing the result in the
 * output buffer provided. The output buffer must be pre-allocated with enough
 * space to store the result. The size needed can be found by calling the
 * \ref base64_decode_size() function.
 *
 * If the output buffer is too small (based on the size provided) then the
 * binary output may be only partially written.
 *
 * @param input The base64 string to encode. This doesn't need to be zero
 *        terminated.
 * @param input_size The size of the input buffer to be converted.
 * @param output A pre-allocated buffer to store the result.
 * @param output_size The size of the allocated buffer, which will be updated
 *        to the number of bytes written to the buffer.
 */
void base64_decode_base64_to_binary(unsigned char const *input, size_t input_size, unsigned char *output, size_t *output_size) {
	size_t size_in;
	size_t size_out;

	size_in = input_size;
	size_out = base64_decode_size(input_size);
	
	if (size_out > *output_size) {
		size_in = base64_encode_size(*output_size - 1) - 1;
	}
	*output_size = base64_decode_size(size_in);

	EVP_DecodeBlock(output, input, size_in);
}

/**
 * Converts a time in epoch format into a day number.
 *
 * Returns the day number for the given epoch time. The epoch time represents
 * the number of seconds since 00:00:00 UTC on 01/01/1970.
 *
 * The day number is calculated as:
 *     (Number of Seconds since Epoch) / (60 * 60 * 24)
 * 
 * @param epoch The epoch time in seconds.
 * @return The day number for this epoch time.
 */
uint32_t epoch_to_day_number(time_t epoch) {
	uint32_t day_number;

	// DayNumber <- (Number of Seconds since Epoch) / (60 * 60 * 24)
	day_number = epoch / (60 * 60 * 24);

	return day_number;
}

/**
 * Converts a time in epoch format into a time interval number.
 *
 * Returns the time interval number for the given epoch time. The epoch time
 * represents the number of seconds since 00:00:00 UTC on 01/01/1970.
 *
 * The time interval number is calculated as:
 *     (Seconds Since Start of DayNumber) / (60 * 10)
 *
 * and must fall in the interval [0, 143].
 *
 * @param epoch The epoch time in seconds.
 * @return The time interval number for this epoch time.
 */
uint8_t epoch_to_time_interval_number(time_t epoch) {
	uint8_t time_interval_number;
	uint32_t day_number;
	uint32_t seconds;

	// TimeNumberInterval <- (Seconds Since Start of DayNumber) / (60 * 10)
	day_number = epoch_to_day_number(epoch);
	seconds = epoch - (day_number * (60 * 60 * 24));

	time_interval_number = seconds / (60 * 10);

	// Falls in interval [0,143]
	if (time_interval_number > 143) {
		time_interval_number = 143;
	}

	return time_interval_number;
}

/** @} addtogroup Utils */

