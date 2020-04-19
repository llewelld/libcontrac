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

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include <openssl/crypto.h>
#include <openssl/hmac.h>
#include <openssl/err.h>

#include "contrac/contrac.h"
#include "contrac/utils.h"
#include "contrac/log.h"

#include "contrac/rpi.h"

// Defines

#define RPI_INFO_PREFIX "CT-RPI"

// Structures

struct _Rpi {
	// Rolling proximity identifier
	unsigned char rpi[RPI_SIZE];
	uint8_t time_interval_number;
};

// Function prototypes

// Function definitions

Rpi * rpi_new() {
	Rpi * data;
	
	data = calloc(sizeof(Rpi), 1);

	return data;
}

void rpi_delete(Rpi * data) {
	if (data) {
		// Clear the data for security
		memset(data, 0, sizeof(Rpi));

		free(data);
	}
}

bool rpi_generate_proximity_id(Rpi * data, Dtk const * dtk, uint8_t time_interval_number) {
	int result = 1;
	unsigned char encode[sizeof(RPI_INFO_PREFIX) + sizeof(uint16_t)];
	unsigned char output[EVP_MAX_MD_SIZE];
	unsigned int out_length = 0;
	unsigned int pos;
	unsigned int min;
	unsigned char const * daily_key;

	// RPI_{i, j} <- Truncate(HMAC(dkt_i, (UTF8("CT-RPI") || TIN_j)), 16)

	if (result > 0) {
		// Produce Info sequence UTF8("CT-DTK") || D_i)
		// From the spec it's not clear whether this is string or byte concatenation.
		// Here we use byte, but it might have to be changed
		memcpy(encode, RPI_INFO_PREFIX, sizeof(RPI_INFO_PREFIX));
		((uint8_t *)(encode + sizeof(RPI_INFO_PREFIX)))[0] = time_interval_number;
		out_length = sizeof(output);

		daily_key = dtk_get_daily_key(dtk);
		HMAC(EVP_sha256(), daily_key, DTK_SIZE, encode, sizeof(encode), output, &out_length);

		_Static_assert ((EVP_MAX_MD_SIZE >= 16), "HMAC buffer size too small");

		// Truncate and copy the result
		min = MIN(out_length, 16);
		for (pos = 0; pos < min; ++pos) {
			data->rpi[pos] = output[pos];
		}
		// Zero out padding if there is any
		for (pos = min; pos < 16; ++pos) {
			data->rpi[pos] = 0;
		}
	}
	
	if (result > 0) {
		data->time_interval_number = time_interval_number;
	}

	if (result <= 0) {
		LOG(LOG_ERR, "Error generating rolling proximity id: %lu\n", ERR_get_error());
	}

	return (result > 0);
}

const unsigned char * rpi_get_proximity_id(Rpi const * data) {
	return data->rpi;
}

uint8_t rpi_get_time_interval_number(Rpi const * data) {
	return data->time_interval_number;
}


void rpi_assign(Rpi * data, unsigned char const * rpi_bytes, uint8_t time_interval_number) {
	memcpy(data->rpi, rpi_bytes, RPI_SIZE);
	data->time_interval_number = time_interval_number;
}

bool rpi_compare(Rpi const * data, Rpi const * comparitor) {
	unsigned char left[RPI_SIZE_BASE64 + 1];
	unsigned char right[RPI_SIZE_BASE64 + 1];
	size_t size;

	size = RPI_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(data->rpi, RPI_SIZE, left, &size);

	size = RPI_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(comparitor->rpi, RPI_SIZE, right, &size);

	return (memcmp(data, comparitor, RPI_SIZE) == 0);
}


