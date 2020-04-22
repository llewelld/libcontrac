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
 * @brief Provides a list of DTKs 
 * @section DESCRIPTION
 *
 * This class allows the simplified management of lists of Dtk objects. This is
 * useful when checking DTKs received from a Diagnosis Server with RPIs
 * captured over Bluetooth. Combined with the \ref RpiList class the two can
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

// Defines

// Structures

/**
 * @brief A DTK list element
 *
 * This is an opaque structure that represents a single item in the list and
 * contains a Dtk instance.
 * 
 * The structure typedef is in dtk_list.h
 */
struct _DtkListItem {
	Dtk * dtk;
	DtkListItem * next;
};

/**
 * @brief The head of a DTK list
 *
 * This is an opaque structure that represents the head of a list of Dtk
 * items.
 * 
 * This is the object usually passed as the first parameter of every non-static
 * function.
 *
 * The structure typedef is in dtk_list.h
 */
struct _DtkList {
	DtkListItem * first;
	DtkListItem * last;
};

// Function prototypes

// Function definitions

/**
 * Creates a new instance of the class.
 *
 * @return The newly created object.
 */
DtkList * dtk_list_new() {
	DtkList * data;
	
	data = calloc(sizeof(DtkList), 1);

	return data;
}

/**
 * Deletes an instance of the class, freeing up the memory allocated to it.
 *
 * This will also delete all items contained in the list.
 *
 * @param data The instance to free.
 */
void dtk_list_delete(DtkList * data) {
	DtkListItem * item;
	DtkListItem * next;

	if (data) {
		item = data->first;
		while (item) {
			next = item->next;			
			dtk_delete(item->dtk);
			free(item);
			item = next;
		}

		free(data);
	}
}

/**
 * Adds an item to the list.
 *
 * This adds a Dtk item to the list. It's primarily for internal use and when
 * adding DTKs to the list it's usually more appropriate to use the
 * \ref dtk_list_add_diagnosis() function.
 *
 * @param data The list to append to.
 * @param dtk The DTK to append.
 */
void dtk_list_append(DtkList * data, Dtk * dtk) {
	DtkListItem * item;
	
	item = calloc(sizeof(DtkListItem), 1);
	item->dtk = dtk;

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
DtkListItem const * dtk_list_first(DtkList const * data) {
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
DtkListItem const * dtk_list_next(DtkListItem const * data) {
	return data->next;
}

/**
 * Returns the Dtk item contained in this list item.
 *
 * @param data The current item in the list.
 * @return The Dtk instance stored in the list element.
 */
Dtk const * dtk_list_get_dtk(DtkListItem const * data) {
	return data->dtk;
}

/**
 * Adds Dtk data to the list.
 *
 * The dtk_bytes buffer passed in must contain exactly DTK_SIZE (16) bytes of
 * data. It doen't have to be null terminated.
 *
 * @param data The current list to operate on.
 * @param dtk_bytes The DTK value to add, in binary format.
 * @param day_number The day number to associate with the DTK.
 */
void dtk_list_add_diagnosis(DtkList * data, unsigned char const * dtk_bytes, uint32_t day_number) {
	Dtk * dtk = dtk_new();
	dtk_assign(dtk, dtk_bytes, day_number);
	dtk_list_append(data, dtk);
}

/** @} addtogroup Containers*/

