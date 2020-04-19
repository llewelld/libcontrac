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
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "contrac/log.h"
#include "contrac/utils.h"
#include "contrac/rpi.h"

#include "contrac/contrac.h"

// Defines

#define STATUS_TK (1 << 0)
#define STATUS_DTK (1 << 1)
#define STATUS_RPI (1 << 2)

#define STATUS_INITIALISED (STATUS_TK | STATUS_DTK | STATUS_RPI)
// Structures

struct _Contrac {
	// Tracing key
	unsigned char tk[TK_SIZE];
	// Daily key
	Dtk * dtk;
	// Rolling proximity identifier
	Rpi * rpi;

	uint32_t day_number;
	uint8_t time_interval_number;
	
	uint32_t status;
};

// Function prototypes

// Function definitions

Contrac * contrac_new() {
	Contrac * data;

	data = calloc(sizeof(Contrac), 1);
	data->dtk = dtk_new();
	data->rpi = rpi_new();

	return data;
}

void contrac_delete(Contrac * data) {
	if (data) {
		dtk_delete(data->dtk);
		rpi_delete(data->rpi);

		// Clear the data for security
		memset(data, 0, sizeof(Contrac));

		free(data);
	}
}

bool contrac_generate_tracing_key(Contrac * data) {
	int result;

	// tk <- CRNG(32)
	result = RAND_bytes(data->tk, TK_SIZE);
	
	if (result == 1) {
		data->status |= STATUS_TK;
	}
	else {
		LOG(LOG_ERR, "Error generating tracing key: %lu\n", ERR_get_error());
	}
	
	return (result == 1);
}

bool contrac_set_day_number(Contrac * data, uint32_t day_number) {
	bool result;

	result = ((data->status & STATUS_TK) != 0);

	if (result) {
		result = contrac_generate_daily_key(data->dtk, data, day_number);
	}

	if (result) {
		data->day_number = day_number;
		data->status |= STATUS_DTK;
	}
	
	return result;
}

bool contrac_set_time_interval_number(Contrac * data, uint8_t time_interval_number) {
	bool result;

	result = ((data->status & STATUS_DTK) != 0);

	if (result) {
		result = rpi_generate_proximity_id(data->rpi, data->dtk, time_interval_number);
	}

	if (result) {
		data->time_interval_number = time_interval_number;
		data->status |= STATUS_RPI;
	}
	
	return result;
}

bool contrac_get_initialised(Contrac const * data) {
	return ((data->status & STATUS_INITIALISED) == STATUS_INITIALISED);
}

void contrac_set_tracing_key(Contrac * data, unsigned char const * tracing_key) {
	memcpy(data->tk, tracing_key, TK_SIZE);
	data->status |= STATUS_TK;
}

const unsigned char * contrac_get_tracing_key(Contrac const * data) {
	return data->tk;
}

// base64 buffer must be at least 45 bytes (TK_SIZE_BASE64 + 1)
void contrac_get_tracing_key_base64(Contrac const * data, char * base64) {
	size_t size = TK_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(data->tk, TK_SIZE, (unsigned char *)base64, &size);

	if (size != (TK_SIZE_BASE64 + 1)) {
		LOG(LOG_ERR, "Base64 tracing key has incorrect size of %d bytes.\n", size);
	}
}

// tracing_key input must be 44 bytes long
bool contrac_set_tracing_key_base64(Contrac * data, char const * tracing_key) {
	bool result = true;
	unsigned char tk[TK_SIZE];
	size_t size;

	if (strlen(tracing_key) != TK_SIZE_BASE64) {
		LOG(LOG_ERR, "Base64 tracing key has incorrect size. Should be %d bytes.\n", TK_SIZE_BASE64);
		result = false;
	}

	if (result) {
		size = TK_SIZE;
		base64_decode_base64_to_binary((unsigned char *)tracing_key, TK_SIZE_BASE64, tk, &size);
		
		if (size < TK_SIZE) {
			LOG(LOG_ERR, "Base64 tracking key output is too short %d bytes.\n", size);
			result = false;
		}
	}

	if (result) {
		contrac_set_tracing_key(data, tk);
	}
	
	return result;
}

const unsigned char * contrac_get_daily_key(Contrac const * data) {
	return dtk_get_daily_key(data->dtk);
}

// base64 buffer must be at least 25 bytes (DTK_SIZE_BASE64 + 1)
void contrac_get_daily_key_base64(Contrac const * data, char * base64) {
	size_t size = DTK_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(dtk_get_daily_key(data->dtk), DTK_SIZE, (unsigned char *)base64, &size);

	if (size != (DTK_SIZE_BASE64 + 1)) {
		LOG(LOG_ERR, "Base64 daily key has incorrect size of %d bytes.\n", size);
	}
}

const unsigned char * contrac_get_proximity_id(Contrac const * data) {
	return rpi_get_proximity_id(data->rpi);
}

// base64 buffer must be at least 25 bytes (RPI_SIZE_BASE64 + 1)
void contrac_get_proximity_id_base64(Contrac const * data, char * base64) {
	size_t size = RPI_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(rpi_get_proximity_id(data->rpi), RPI_SIZE, (unsigned char *)base64, &size);

	if (size != (RPI_SIZE_BASE64 + 1)) {
		LOG(LOG_ERR, "Base64 proximity id has incorrect size of %d bytes.\n", size);
	}
}






