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
 * @brief Provides a list of RPIs 
 * @section DESCRIPTION
 *
 * This class allows the simplified management of lists of Rpi objects. This is
 * useful when checking DTKs received from a Diagnosis Server with RPIs
 * captured over Bluetooth. Combined with the \ref DtkList class the two can
 * be easily stored and passed into the \ref match_list_find_matches() function.
 *
 */

/** \addtogroup Containers
 *  @{
 */

#ifndef __RPI_LIST_H
#define __RPI_LIST_H

// Includes

#include "contrac/contrac.h"
#include "contrac/rpi.h"

// Defines

// Structures

/**
 * An opaque structure that represents the head of the list.
 * 
 * The internal structure can be found in rpi_list.c
 */
typedef struct _RpiList RpiList;

/**
 * An opaque structure that represents an item in the list.
 * 
 * The internal structure can be found in rpi_list.c
 */
typedef struct _RpiListItem RpiListItem;

// Function prototypes

RpiList * rpi_list_new();
void rpi_list_delete(RpiList * data);

void rpi_list_append(RpiList * data, Rpi * rpi);
void rpi_list_add_beacon(RpiList * data, unsigned char const * rpi_bytes, uint8_t time_interval_number);

RpiListItem const * rpi_list_first(RpiList const * data);
RpiListItem const * rpi_list_next(RpiListItem const * data);
Rpi const * rpi_list_get_rpi(RpiListItem const * data);

// Function definitions

#endif // __RPI_LIST_H

/** @} addtogroup Containers*/

