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
#include <openssl/kdf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include "contrac/contrac.h"
#include "contrac/utils.h"
#include "contrac/log.h"

#include "contrac/dtk.h"

// Defines

#define DTK_INFO_PREFIX "CT-DTK"

// Structures

struct _Dtk {
	// Daily key
	unsigned char dtk[DTK_SIZE];
	uint32_t day_number;
};

// Function prototypes

// Function definitions

Dtk * dtk_new() {
	Dtk * data;
	
	data = calloc(sizeof(Dtk), 1);

	return data;
}

void dtk_delete(Dtk * data) {
	if (data) {
		// Clear the data for security
		memset(data, 0, sizeof(Dtk));

		free(data);
	}
}

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


