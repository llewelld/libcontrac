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

// Includes

#include <openssl/evp.h>

// Defines

// Structures

// Function prototypes

// Function definitions

// Function definitions

size_t base64_encode_size(size_t binary_input) {
	return (((size_t)((binary_input + 2) / 3)) * 4) + 1;
}

size_t base64_decode_size(size_t base64_input) {
	return (((size_t)((base64_input + 3) / 4)) * 3) + 1;
}

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

