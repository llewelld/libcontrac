/** \ingroup RandomProximityIdentifier
 * @file
 * @author	David Llewellyn-Jones <david@flypig.co.uk>
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2020
 * Released under the GPLv2.
 *
 * @brief Random Proximity Identifier functionality
 * @section DESCRIPTION
 *
 * This class is used to generate and manage the Random Proximity Identifier
 * (RPI). It's largely internal. The functionality from \ref Contrac should
 * generally be used in preference to this.
 *
 */

/** \addtogroup RandomProximityIdentifier
 *  @{
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

/**
 * Used internally.
 *
 * This is the prefix for the Info parameter provided to the HMAC and used to
 * generate the RPI.
 */
#define RPI_INFO_PREFIX "CT-RPI"

// Structures

/**
 * @brief The structure used to represent a Rolling Proximity Identifier.
 *
 * This is an opaque structure that contains information about the RPI..
 *
 * This must be passed as the first parameter of every non-static function.
 *
 * The structure typedef is in dtk.h
 */
struct _Rpi {
	// Rolling proximity identifier
	unsigned char rpi[RPI_SIZE];
	uint8_t time_interval_number;
};

// Function prototypes

// Function definitions

/**
 * Creates a new instance of the class.
 *
 * @return The newly created object.
 */
Rpi * rpi_new() {
	Rpi * data;
	
	data = calloc(sizeof(Rpi), 1);

	return data;
}

/**
 * Deletes an instance of the class, freeing up the memory allocated to it.
 *
 * @param data The instance to free.
 */
void rpi_delete(Rpi * data) {
	if (data) {
		// Clear the data for security
		memset(data, 0, sizeof(Rpi));

		free(data);
	}
}

/**
 * Generates a Rolling Proximity Identifier based on the time interval number
 * provided.
 *
 * The operation may fail under certain circumstances, such as if the
 * HMAC operation fails for some reason.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_set_time_interval_number() function instead.
 *
 * @param data The context object to work with.
 * @param time_interval_number The time interval number to use to generate the
 *        key.
 * @return true if the operation completed successfully, false otherwise.
 */
bool rpi_generate_proximity_id(Rpi * data, Dtk const * dtk, uint8_t time_interval_number) {
	int result = 1;
	unsigned char encode[sizeof(RPI_INFO_PREFIX) + sizeof(time_interval_number)];
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

/**
 * Gets the Rolling Proximity Identifier for the device in binary format.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_get_proximity_id() function instead.
 *
 * This allows the Rolling Proximity Identifier to be extracted. The Rolling
 * Proximity Identifier is public, in the sense that it is usual to broadcast
 * the value in Bluetooth beacons.
 *
 * The buffer returned will contain exactly RPI_SIZE (16) bytes of data in
 * binary format. This may therefore contain null bytes, and the buffer will not
 * necessarily be null terminated. Future operations may cause the data to
 * change, so the caller should make a copy of the buffer rather than keeping
 * a pointer to it.
 *
 * @param data The context object to work with.
 * @return The Rolling Proximity Identifier in binary format, not null
 *         terminated.
 */
unsigned char const * rpi_get_proximity_id(Rpi const * data) {
	return data->rpi;
}

/**
 * Gets the time interval number that applies to the current RPI.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_set_time_interval_number() function instead.
 *
 * @param data The context object to work with.
 * @return The time interval number stored in the object.
 */
uint8_t rpi_get_time_interval_number(Rpi const * data) {
	return data->time_interval_number;
}

/**
 * Populates the data structure.
 *
 * Allows the RPI and time interval number values of the object to be set
 * explicitly.
 *
 * For internal use. To set the RPI it generally makes more sense to use one of
 * eiher contrac_set_time_interval_number() or contrac_update_current_time()
 * instead.
 *
 * The rpi_bytes buffer passed in must contain exactly RPI_SIZE (16) bytes of
 * data. It doen't have to be null terminated.
 *
 * @param data The context object to work with.
 * @param rpi_bytes The RPI value to set, in binary format.
 * @param day_number The time interval number to associate with this RPI.
 */
void rpi_assign(Rpi * data, unsigned char const * rpi_bytes, uint8_t time_interval_number) {
	memcpy(data->rpi, rpi_bytes, RPI_SIZE);
	data->time_interval_number = time_interval_number;
}

/**
 * Compares two RPI values.
 *
 * @param data The RPI to compare with.
 * @param comparitor The RPI to compare against.
 * @return true if the two RPIs are the same, false otherwise.
 */
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

/** @} addtogroup RandomProximityIdentifier */

