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

// Includes

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "contrac/contrac.h"
#include "contrac/utils.h"
#include "contrac/log.h"
#include "contrac/dtk_list.h"

#include "contrac/rpi_list.h"

// Defines

// Structures

/**
 * @brief An RPI list element
 *
 * This is an opaque structure that represents a single item in the list and
 * contains an Rpi instance.
 * 
 * The structure typedef is in rpi_list.h
 */
struct _RpiListItem {
	Rpi * rpi;
	RpiListItem * next;
};

/**
 * @brief The head of an RPI list
 *
 * This is an opaque structure that represents the head of a list of Rpi
 * items.
 * 
 * This is the object usually passed as the first parameter of every non-static
 * function.
 *
 * The structure typedef is in dtk_list.h
 */
struct _RpiList {
	RpiListItem * first;
	RpiListItem * last;
};

// Function prototypes

// Function definitions

/**
 * Creates a new instance of the class.
 *
 * @return The newly created object.
 */
RpiList * rpi_list_new() {
	RpiList * data;
	
	data = calloc(sizeof(RpiList), 1);

	return data;
}

/**
 * Deletes an instance of the class, freeing up the memory allocated to it.
 *
 * This will also delete all items contained in the list.
 *
 * @param data The instance to free.
 */
void rpi_list_delete(RpiList * data) {
	RpiListItem * item;
	RpiListItem * next;

	if (data) {
		item = data->first;
		while (item) {
			next = item->next;
			rpi_delete(item->rpi);
			free(item);
			item = next;
		}

		free(data);
	}
}

/**
 * Adds an item to the list.
 *
 * This adds an Rpi item to the list. It's primarily for internal use and when
 * adding RPIs to the list it's usually more appropriate to use the
 * \ref rpi_list_add_beacon() function.
 *
 * @param data The list to append to.
 * @param rpi The RPI to append.
 */
void rpi_list_append(RpiList * data, Rpi * rpi) {
	RpiListItem * item;
	
	item = calloc(sizeof(RpiListItem), 1);
	item->rpi = rpi;

	if (data->last == NULL) {
		data->first = item;
		data->last = item;
	}
	else {
		data->last->next = item;
		data->last = item;
	}
}

/**
 * Returns the first item in the list.
 *
 * Useful for iterating through the items in the list.
 *
 * @param data The list to operate on.
 * @return The first item of the list.
 */
RpiListItem const * rpi_list_first(RpiList const * data) {
	return data->first;
}

/**
 * Returns the next item in the list.
 *
 * Useful for iterating through the items in the list.
 *
 * @param data The current item in the list.
 * @return The next item in the list following the current item.
 */
RpiListItem const * rpi_list_next(RpiListItem const * data) {
	return data->next;
}

/**
 * Returns the Rpi item contained in this list item.
 *
 * @param data The current item in the list.
 * @return The Rpi instance stored in the list element.
 */
Rpi const * rpi_list_get_rpi(RpiListItem const * data) {
	return data->rpi;
}

/**
 * Adds Rpi data to the list.
 *
 * The rpi_bytes buffer passed in must contain exactly RPI_SIZE (16) bytes of
 * data. It doen't have to be null terminated.
 *
 * @param data The current list to operate on.
 * @param rpi_bytes The RPI value to add, in binary format.
 * @param time_interval_number The time interval number to associate with the
 *        RPI.
 */
void rpi_list_add_beacon(RpiList * data, unsigned char const * rpi_bytes, uint8_t time_interval_number) {
	Rpi * rpi = rpi_new();
	rpi_assign(rpi, rpi_bytes, time_interval_number);
	rpi_list_append(data, rpi);
}

/** @} addtogroup Containers*/

