/** \ingroup DailyTracingKey
 * @file
 * @author	David Llewellyn-Jones <david@flypig.co.uk>
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2020
 * Released under the GPLv2.
 *
 * @brief Daily Tracing Key functionality
 * @section DESCRIPTION
 *
 * This class is used to generate and manage the Daily Tracing Key (DTK). It's
 * largely internal. The functionality from \ref Contrac should generally be
 * used in preference to this.
 *
 */

/** \addtogroup DailyTracingKey
 *  @{
 */

// Includes

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include <openssl/crypto.h>
#include <openssl/kdf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include "contrac/contrac.h"
#include "contrac/utils.h"
#include "contrac/log.h"

#include "contrac/dtk.h"

// Defines

/**
 * Used internally.
 *
 * This is the prefix for the Info parameter provided to the HKDF and used to
 * generate the DTK.
 */
#define DTK_INFO_PREFIX "CT-DTK"

// Structures

/**
 * @brief The structure used to represent a Daily Tracing Key.
 *
 * This is an opaque structure that contains information about the DTK..
 *
 * This must be passed as the first parameter of every non-static function.
 *
 * The structure typedef is in dtk.h
 */
struct _Dtk {
	// Daily key
	unsigned char dtk[DTK_SIZE];
	uint32_t day_number;
};

// Function prototypes

// Function definitions

/**
 * Creates a new instance of the class.
 *
 * @return The newly created object.
 */
Dtk * dtk_new() {
	Dtk * data;
	
	data = calloc(sizeof(Dtk), 1);

	return data;
}

/**
 * Deletes an instance of the class, freeing up the memory allocated to it.
 *
 * @param data The instance to free.
 */
void dtk_delete(Dtk * data) {
	if (data) {
		// Clear the data for security
		memset(data, 0, sizeof(Dtk));

		free(data);
	}
}

/**
 * Generates a Daily Tracing Key based on the day number provided.
 *
 * The operation may fail under certain circumstances, such as if the
 * HKDF operation fails for some reason.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_set_day_number() function instead.
 *
 * @param data The context object to work with.
 * @param day_number The day number to use to generate the key.
 * @return true if the operation completed successfully, false otherwise.
 */
bool dtk_generate_daily_key(Dtk * data, Contrac const * contrac, uint32_t day_number) {
	int result = 1;
	char encode[sizeof(DTK_INFO_PREFIX) + sizeof(day_number)];
	size_t out_length = 0;
	EVP_PKEY_CTX *pctx = NULL;
	unsigned char const * tk;

	// dtk_i <- HKDF(tk, NULL, (UTF8("CT-DTK") || D_i), 16)

	if (result > 0) {
		// Produce Info sequence UTF8("CT-DTK") || D_i)
		// From the spec it's not clear whether this is string or byte concatenation.
		// Here we use byte, but it might have to be changed
		memcpy(encode, DTK_INFO_PREFIX, sizeof(DTK_INFO_PREFIX));
		((uint32_t *)(encode + sizeof(DTK_INFO_PREFIX)))[0] = day_number;

		pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

		result = EVP_PKEY_derive_init(pctx);
	}

	if (result > 0) {
		result = EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256());
	}

	if (result > 0) {
		result = EVP_PKEY_CTX_set1_hkdf_salt(pctx, NULL, 4);
	}
	
	if (result > 0) {
		tk = contrac_get_tracing_key(contrac);
		result = EVP_PKEY_CTX_set1_hkdf_key(pctx, tk, TK_SIZE);
	}
	
	if (result > 0) {
		result = EVP_PKEY_CTX_add1_hkdf_info(pctx, encode, sizeof(encode));
	}
	
	if (result > 0) {
		out_length = DTK_SIZE;
		result = EVP_PKEY_derive(pctx, data->dtk, &out_length);
	}
	
	if ((result > 0) && (out_length == DTK_SIZE)) {
		data->day_number = day_number;
		result = 1;
	}

	if (result <= 0) {
		LOG(LOG_ERR, "Error generating daily key: %lu\n", ERR_get_error());
	}
	
	// Freeing a NULL value is safe
	EVP_PKEY_CTX_free(pctx);

	return (result > 0);
}

/**
 * Gets the Daily Tracing Key for the device in binary format.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_get_daily_key() function instead.
 *
 * This allows the Daily Tracing Key to be extracted. The Daily Tracing Key
 * should be kept secret (to maintain privacy) until a positive test is
 * confirmed, at which point the user may choose to upload the key to a
 * Diagnosis Server, so that others can be notified.
 *
 * The buffer returned will contain exactly DTK_SIZE (16) bytes of data in
 * binary format. This may therefore contain null bytes, and the buffer will not
 * necessarily be null terminated. Future operations may cause the data to
 * change, so the caller should make a copy of the buffer rather than keeping
 * a pointer to it.
 *
 * @param data The context object to work with.
 * @return The Daily Tracing Key in binary format, not null terminated.
 */
unsigned char const * dtk_get_daily_key(Dtk const * data) {
	return data->dtk;
}

/**
 * Gets the day number that applies to the current DTK.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_get_day_number() function instead.
 *
 * @param data The context object to work with.
 * @return The day number stored in the object..
 */
uint32_t dtk_get_day_number(Dtk const * data) {
	return data->day_number;
}

/**
 * Populates the data structure.
 *
 * Allows the DTK and day number values of the object to be set explicitly.
 *
 * For internal use. To set the DTK it generally makes more sense to use one of
 * eiher contrac_set_day_number() or contrac_update_current_time() instead.
 *
 * The dtk_bytes buffer passed in must contain exactly DTK_SIZE (16) bytes of
 * data. It doen't have to be null terminated.
 *
 * @param data The context object to work with.
 * @param dtk_bytes The DTK value to set, in binary format.
 * @param day_number The day number to associate with this DTK.
 */
void dtk_assign(Dtk * data, unsigned char const * dtk_bytes, uint32_t day_number) {
	memcpy(data->dtk, dtk_bytes, DTK_SIZE);
	data->day_number = day_number;
}

/** @} addtogroup DailyTracingKey */

