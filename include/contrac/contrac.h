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

#ifndef __CONTRAC_H
#define __CONTRAC_H

// Includes

#include <stdint.h>
#include <stdbool.h>

// Defines

// Data sizes in bytes

/**
 * The size in bytes of a Tracing Key in binary format
 */
#define TK_SIZE (32)

/**
 * The size in bytes of a Tracing Key in base64 format
 */
#define TK_SIZE_BASE64 (44)

// Structures

/**
 * An opaque structure for storing the main state.
 * 
 * The internal structure can be found in contrac.c
 */
typedef struct _Contrac Contrac;

// Function prototypes

Contrac * contrac_new();
void contrac_delete(Contrac * data);

bool contrac_generate_tracing_key(Contrac * data);
bool contrac_set_day_number(Contrac * data, uint32_t day_number);
bool contrac_set_time_interval_number(Contrac * data, uint8_t time_interval_number);
bool contrac_update_current_time(Contrac * data);
bool contrac_get_initialised(Contrac const * data);

unsigned char const * contrac_get_tracing_key(Contrac const * data);
void contrac_get_tracing_key_base64(Contrac const * data, char * base64);

void contrac_set_tracing_key(Contrac * data, unsigned char const * tracing_key);
bool contrac_set_tracing_key_base64(Contrac * data, char const * tracing_key);

unsigned char const * contrac_get_daily_key(Contrac const * data);
void contrac_get_daily_key_base64(Contrac const * data, char * base64);

unsigned char const * contrac_get_proximity_id(Contrac const * data);
void contrac_get_proximity_id_base64(Contrac const * data, char * base64);

// Function definitions

#endif // __CONTRAC_H

/** @} addtogroup KeyGeneration */

