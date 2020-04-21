/** \ingroup KeyGeneration
 * @file
 * @author	David Llewellyn-Jones <david@flypig.co.uk>
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2020
 * Released under the GPLv2.
 *
 * @brief Core Contact Tracing functionality 
 * @section DESCRIPTION
 *
 * This class provides the core Contact Tracing functionality. It provides an
 * interfaces for:
 * 1. Generating a random Tracing Key.
 * 2. Generating a Daily Tracing Key based on the current day number.
 * 3. Generating a Rolling Proximity Identifier based on the current time
 *    interval number.
 *
 * Values can be extracted and set in binary or base64 format.
 *
 */

/** \addtogroup KeyGeneration
 *  @{
 */

// Includes

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "contrac/log.h"
#include "contrac/utils.h"
#include "contrac/rpi.h"

#include "contrac/contrac.h"

// Defines

/**
 * Inernal flag mask.
 * 
 * When set the flag indicates that the Tracing Key has been correctly
 * initialised.
 */
#define STATUS_TK (1 << 0)

/**
 * Inernal flag mask.
 * 
 * When set the flag indicates that the Daily Tracing Key has been correctly
 * initialised.
 */
#define STATUS_DTK (1 << 1)

/**
 * Inernal flag mask.
 * 
 * When set the flag indicates that the Rolling Proximity Identifier has been
 * correctly initialised.
 */
#define STATUS_RPI (1 << 2)

/**
 * Inernal flag mask.
 * 
 * When all of these flags are set it indicates that the structure is fully
 * initialised.
 * .
 */
#define STATUS_INITIALISED (STATUS_TK | STATUS_DTK | STATUS_RPI)
// Structures

/**
 * @brief The core structure for storing Contact Tracing state.
 *
 * This is an opaque structure that contains the core state for the library.
 * 
 * This must be passed as the first parameter of every non-static function.
 * 
 * The structure typedef is in contrac.h
 */
struct _Contrac {
	// Tracing key
	unsigned char tk[TK_SIZE];
	// Daily key
	Dtk * dtk;
	// Rolling proximity identifier
	Rpi * rpi;

	uint32_t status;
};

// Function prototypes

// Function definitions

/**
 * Create a new instance of the class.
 *
 * @return The newly created object.
 */
Contrac * contrac_new() {
	Contrac * data;

	data = calloc(sizeof(Contrac), 1);
	data->dtk = dtk_new();
	data->rpi = rpi_new();

	return data;
}

/**
 * Delete an instance of the class, freeing up the memory allocated to it.
 *
 * @param data The instance to free.
 */
void contrac_delete(Contrac * data) {
	if (data) {
		dtk_delete(data->dtk);
		rpi_delete(data->rpi);

		// Clear the data for security
		memset(data, 0, sizeof(Contrac));

		free(data);
	}
}

/**
 * Generate a random Contact Tracing Key.
 *
 * The operation may fail under certain circumstances, such as there being
 * insufficient entropy in the system to guarantee a random result.
 *
 * @param data The context object to work with.
 * @return true if the operation completed successfully, false otherwise.
 */
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

/**
 * Set the current day number.
 *
 * This will result in a new Daily Tracing Key being generated based on the
 * day provided. If neither the Tracing Key nor the day have changed, the DTK
 * will remain the same.
 *
 * The day number is calculated as:
 *     (Number of Seconds since Epoch) / (60 * 60 * 24)
 * 
 * Which can be caluclated from the current epoch using the
 * epoch_to_day_number() function.
 *
 * The operation may fail if a Tracing Key has yet to be configured.
 *
 * @param data The context object to work with.
 * @return true if the operation completed successfully, false otherwise.
 */
bool contrac_set_day_number(Contrac * data, uint32_t day_number) {
	bool result;

	result = ((data->status & STATUS_TK) != 0);

	if (result) {
		result = dtk_generate_daily_key(data->dtk, data, day_number);
	}

	if (result) {
		data->status |= STATUS_DTK;
	}
	
	return result;
}

/**
 * Set the current time interval number.
 *
 * This will result in a new Rolling Proximity Idetifier being generated based 
 * on the time interval number. If none of the Tracing Key, day nor time 
 * interval have changed, the RPI will stay the same.
 *
 * The time interval number is calculated as:
 *     (Seconds Since Start of DayNumber) / (60 * 10)
 *
 * and must fall in the interval [0, 143].
 *
 * It can be caluclated from the current epoch using the
 * epoch_to_time_interval_number() function.
 *
 * The operation may fail if a Tracing Key or Daily Tracing Key have yet to be
 * configured.
 *
 * @param data The context object to work with.
 * @param time_interval_number The time interval number to set.
 * @return true if the operation completed successfully, false otherwise.
 */
bool contrac_set_time_interval_number(Contrac * data, uint8_t time_interval_number) {
	bool result;

	result = ((data->status & STATUS_DTK) != 0);

	if (result) {
		result = rpi_generate_proximity_id(data->rpi, data->dtk, time_interval_number);
	}

	if (result) {
		data->status |= STATUS_RPI;
	}
	
	return result;
}

/**
 * Get whether the internal state has been fully configured or not.
 *
 * The internal state must be fully configured before a Daily Tracing Key or
 * Rolling Proximity Identifier can be calculated. This function returns whether
 * it is in this state or not.
 *
 * In order to fully configure the structure, a Tracing Key must either be
 * generated using contrac_generate_tracing_key(), or set using either
 * contrac_set_tracing_key() or contrac_set_tracing_key_base64().
 *
 * In addition the day number and time interval number must be set using
 * contrac_set_day_number() and contrac_set_time_interval_number() respectively.
 * 
 * Alternatively these can be set automatically based on the current time using
 * contrac_update_current_time().
 *
 * @param data The context object to work with.
 * @return true if the state is fully initialised, false otherwise.
 */
bool contrac_get_initialised(Contrac const * data) {
	return ((data->status & STATUS_INITIALISED) == STATUS_INITIALISED);
}

/**
 * Set the Tracing Key for the device in binary format.
 *
 * When first configuring a system, the Tracing Key must be generated
 * randomly, e.g. using contrac_generate_tracing_key().
 *
 * However, on future runs it's important that the Tracing Key stays the same.
 * In this case the key can be restored using this function.
 *
 * The tracing_key buffer passed in must contain exactly TK_SIZE (32) bytes of
 * data.
 *
 * @param data The context object to work with.
 * @param tracing_key The Tracing Key to set in binary format.
 */
void contrac_set_tracing_key(Contrac * data, unsigned char const * tracing_key) {
	memcpy(data->tk, tracing_key, TK_SIZE);
	data->status |= STATUS_TK;
}

/**
 * Get the Tracing Key for the device in binary format.
 *
 * This allows the Tracing Key to be extracted. The Tracing Key should be kept
 * secret (to maintain privacy), however it still may need to be extracted, for
 * example so it can be saved in persistent storage between runs.
 *
 * The buffer returned will contain exactly TK_SIZE (32) bytes of data in binary
 * format. This may therefore contain null bytes, and the buffer will not 
 * necessarily be null terminated.
 *
 * @param data The context object to work with.
 * @return The Tracing Key in binary format, not null terminated.
 */
unsigned char const * contrac_get_tracing_key(Contrac const * data) {
	return data->tk;
}

/**
 * Get the Tracing Key for the device in base64 format.
 *
 * This allows the Tracing Key to be extracted. The Tracing Key should be kept
 * secret (to maintain privacy), however it still may need to be extracted, for
 * example so it can be saved in persistent storage between runs.
 *
 * The buffer provided must be at least TK_SIZE_BAS64 + 1 (45) bytes long and
 * will be filled out with the Tracing Key in base64 format (TK_SIZE_BAS64
 * bytes) followed by a null terminator (1 byte).
 *
 * @param data The context object to work with.
 * @param base64 A buffer of at least TK_SIZE_BAS64 + 1 bytes for the result.
 */
void contrac_get_tracing_key_base64(Contrac const * data, char * base64) {
	size_t size = TK_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(data->tk, TK_SIZE, (unsigned char *)base64, &size);

	if (size != (TK_SIZE_BASE64 + 1)) {
		LOG(LOG_ERR, "Base64 tracing key has incorrect size of %d bytes.\n", size);
	}
}

/**
 * Set the Tracing Key for the device in base64 format.
 *
 * When first configuring a system, the Tracing Key must be generated
 * randomly, e.g. using contrac_generate_tracing_key().
 *
 * However, on future runs it's important that the Tracing Key stays the same.
 * In this case the key can be restored using this function.
 *
 * The tracing_key buffer passed in must contain exactly TK_SIZE_BASE64 (44)
 * bytes of base64-encoded data. It can be null terminated, but doesn't need to
 * be.
 *
 * @param data The context object to work with.
 * @param tracing_key The Tracing Key to set in base64 format.
 */
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

/**
 * Get the Daily Tracing Key for the device in binary format.
 *
 * This allows the Daily Tracing Key to be extracted. The Daily Tracing Key
 * should be kept secret (to maintain privacy) until a positive test is
 * confirmed, at which point the user may choose to upload the key to a
 * Diagnosis Server, so that others can be notified.
 *
 * The buffer returned will contain exactly DTK_SIZE (16) bytes of data in
 * binary format. This may therefore contain null bytes, and the buffer will not 
 * necessarily be null terminated.
 *
 * @param data The context object to work with.
 * @return The Daily Tracing Key in binary format, not null terminated.
 */
unsigned char const * contrac_get_daily_key(Contrac const * data) {
	return dtk_get_daily_key(data->dtk);
}

/**
 * Get the Daily Tracing Key for the device in base64 format.
 *
 * This allows the Daily Tracing Key to be extracted. The Daily Tracing Key
 * should be kept secret (to maintain privacy) until a positive test is
 * confirmed, at which point the user may choose to upload the key to a
 * Diagnosis Server, so that others can be notified.
 *
 * The buffer provided must be at least DTK_SIZE_BASE64 + 1 (25) bytes long and
 * will be filled out with the Tracing Key in base64 format (DTK_SIZE_BASE64
 * bytes) followed by a null terminator (1 byte).
 *
 * @param data The context object to work with.
 * @param base64 A buffer of at least DTK_SIZE_BASE64 + 1 bytes for the result.
 */
void contrac_get_daily_key_base64(Contrac const * data, char * base64) {
	size_t size = DTK_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(dtk_get_daily_key(data->dtk), DTK_SIZE, (unsigned char *)base64, &size);

	if (size != (DTK_SIZE_BASE64 + 1)) {
		LOG(LOG_ERR, "Base64 daily key has incorrect size of %d bytes.\n", size);
	}
}

/**
 * Get the Rolling Proximity Identifier for the device in binary format.
 *
 * This allows the Rolling Proximity Identifier to be extracted. The Rolling
 * Proximity Identifier is for broadcast to other devices using BLE and changes
 * frequently.
 *
 * The buffer returned will contain exactly RPI_SIZE (16) bytes of data in
 * binary format. This may therefore contain null bytes, and the buffer will not 
 * necessarily be null terminated.
 *
 * @param data The context object to work with.
 * @return The Rolling Proximity Identifier in binary format, not null 
 *         terminated.
 */
unsigned char const * contrac_get_proximity_id(Contrac const * data) {
	return rpi_get_proximity_id(data->rpi);
}

/**
 * Get the Rolling Proximity Identifier for the device in base64 format.
 *
 * This allows the Rolling Proximity Identifier to be extracted. The Rolling
 * Proximity Identifier is for broadcast to other devices using BLE and changes
 * frequently.
 *
 * The buffer provided must be at least RPI_SIZE_BASE64 + 1 (25) bytes long and
 * will be filled out with the Tracing Key in base64 format (RPI_SIZE_BASE64
 * bytes) followed by a null terminator (1 byte).
 *
 * @param data The context object to work with.
 * @param base64 A buffer of at least RPI_SIZE_BASE64 + 1 bytes for the result.
 */
void contrac_get_proximity_id_base64(Contrac const * data, char * base64) {
	size_t size = RPI_SIZE_BASE64 + 1;
	base64_encode_binary_to_base64(rpi_get_proximity_id(data->rpi), RPI_SIZE, (unsigned char *)base64, &size);

	if (size != (RPI_SIZE_BASE64 + 1)) {
		LOG(LOG_ERR, "Base64 proximity id has incorrect size of %d bytes.\n", size);
	}
}

/**
 * Update the Daily Tracing Key and Random Proxmity Identifer.
 *
 * The Daily Tracing Key changes every day, the Random Proximity Identifier
 * changes every 10 minutes.
 *
 * Calling this function will update them both based on the current system
 * time.
 *
 * Note that getting either the DTK or RPI does not cause an update, so if you
 * want to get the correct values based on the time, it makes sense to call
 * this function before getting them.
 *
 * The operation may fail if the state has not yet been fully initialised (for
 * example if a Tracing Key has not yet been generated or set).
 *
 * @param data The context object to work with.
 * @return true if the operation completed successfully, false otherwise.
 */
bool contrac_update_current_time(Contrac * data) {
	bool result;
	time_t epoch;
	uint32_t dn_stored;
	uint32_t dn_now;
	uint8_t tn_stored;
	uint8_t tn_now;

	result = true;

	if ((data->status & STATUS_TK) == 0) {
		// No Tracing Key has been set, so generate a random key
		result = contrac_generate_tracing_key(data);
	}

	epoch = time(NULL);

	if (result) {
		dn_now = epoch_to_day_number(epoch);
		dn_stored = dtk_get_day_number(data->dtk);

		// Only set again if uninitialised or the time has changed
		if ((dn_now != dn_stored) || ((data->status & STATUS_DTK) == 0)) {
			result = contrac_set_day_number(data, dn_now);
		}
	}

	if (result) {
		tn_now = epoch_to_time_interval_number(epoch);
		tn_stored= rpi_get_time_interval_number(data->rpi);

		// Only set again if uninitialised or the time has changed
		if ((tn_now != tn_stored) || (dn_now != dn_stored) || ((data->status & STATUS_RPI) == 0)) {
			result = contrac_set_time_interval_number(data, tn_now);
		}
	}

	return result;
}

/** @} addtogroup KeyGeneration */

