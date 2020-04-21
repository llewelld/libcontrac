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
 * This class is used to generate and manage the Daily Tracing Key. It's
 * largely internal. The functionality from \ref Contrac should generally be
 * used instead of these functions.
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
 * Create a new instance of the class.
 *
 * @return The newly created object.
 */
Dtk * dtk_new() {
	Dtk * data;
	
	data = calloc(sizeof(Dtk), 1);

	return data;
}

/**
 * Delete an instance of the class, freeing up the memory allocated to it.
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
 * Generate a random Daily Tracing Key.
 *
 * The operation may fail under certain circumstances, such as if the
 * HKDF operation fails for some reason.
 *
 * For internal use. It generally makes more sense to use the
 * contrac_set_day_number() function instead.
 *
 * @param data The context object to work with.
 * @return true if the operation completed successfully, false otherwise.
 */
bool dtk_generate_daily_key(Dtk * data, Contrac const * contrac, uint32_t day_number) {
	int result = 1;
	char encode[sizeof(DTK_INFO_PREFIX) + sizeof(day_number)];
	size_t out_length = 0;
	EVP_PKEY_CTX *pctx = NULL;
	const unsigned char * tk;

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

const unsigned char * dtk_get_daily_key(Dtk const * data) {
	return data->dtk;
}

uint32_t dtk_get_day_number(Dtk const * data) {
	return data->day_number;
}

void dtk_assign(Dtk * data, unsigned char const * dtk_bytes, uint32_t day_number) {
	memcpy(data->dtk, dtk_bytes, DTK_SIZE);
	data->day_number = day_number;
}

/** @} addtogroup DailyTracingKey */

