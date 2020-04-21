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

#ifndef __DTK_H
#define __DTK_H

// Includes

// Defines

/**
 * The size in bytes of a DTK in binary format.
 *
 */
#define DTK_SIZE (16)

/**
 * The size in bytes of a DTK in base64 format, not including the null
 * terminator.
 *
 */
#define DTK_SIZE_BASE64 (24)

// Structures

/**
 * An opaque structure for representing a DTK.
 *
 * The internal structure can be found in dtk.c
 */
typedef struct _Dtk Dtk;

// Function prototypes

Dtk * dtk_new();
void dtk_delete(Dtk * data);

bool dtk_generate_daily_key(Dtk * data, Contrac const * contrac, uint32_t day_number);
const unsigned char * dtk_get_daily_key(Dtk const * data);
uint32_t dtk_get_day_number(Dtk const * data);
void dtk_assign(Dtk * data, unsigned char const * dtk_bytes, uint32_t day_number);

// Function definitions

#endif // __DTK_H

/** @} addtogroup DailyTracingKey */

