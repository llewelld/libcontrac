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

#ifndef __RPI_H
#define __RPI_H

// Includes

#include "contrac/contrac.h"
#include "contrac/dtk.h"

// Defines

/**
 * The size in bytes of an RPI in binary format.
 *
 */
#define RPI_SIZE (16)

/**
 * The size in bytes of an RPI in base64 format, not including the null
 * terminator.
 *
 */
#define RPI_SIZE_BASE64 (24)

/**
 * The maximum value a time interval number can take. Time interval numbers
 * are measured from the start of the day and increment every 10 minutes, so
 * must fall within the interval [0, 143].
 *
 */
#define RPI_INTERVAL_MAX (144)

// Structures

/**
 * An opaque structure for representing a DTK.
 *
 * The internal structure can be found in rpi.c
 */
typedef struct _Rpi Rpi;

// Function prototypes

Rpi * rpi_new();
void rpi_delete(Rpi * data);

bool rpi_generate_proximity_id(Rpi * data, Dtk const * dtk, uint8_t time_interval_number);
unsigned char const * rpi_get_proximity_id(Rpi const * data);
uint8_t rpi_get_time_interval_number(Rpi const * data);
void rpi_assign(Rpi * data, unsigned char const * rpi_bytes, uint8_t time_interval_number);
bool rpi_compare(Rpi const * data, Rpi const * comparitor);

// Function definitions

#endif // __RPI_H

/** @} addtogroup RandomProximityIdentifier */

