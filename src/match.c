/** \ingroup Matching
 * @file
 * @author	David Llewellyn-Jones <david@flypig.co.uk>
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2020
 * Released under the GPLv2.
 *
 * @brief Provides a way to match collected RPIs with downloaded DTKs.
 * @section DESCRIPTION
 *
 * This class provides functionality allowing RPIs that have been collected
 * over Bluetooth to be matched against DTKs downloaded from a Diagnosis
 * Server.
 *
 * The list of RPIs and DTKs can be constructed easily using the
 * \ref Container functions.
 *
 */

/** \addtogroup Matching
 *  @{
 */

// Includes

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include <openssl/crypto.h>
#include <openssl/hmac.h>
#include <openssl/err.h>

#include "contrac/contrac.h"
#include "contrac/utils.h"
#include "contrac/log.h"
#include "contrac/rpi_list.h"
#include "contrac/dtk_list.h"

#include "contrac/match.h"

// Defines

// Structures

/**
 * @brief A match list element
 *
 * This is an opaque structure that represents a single item in the list and
 * captures a match between an RPI and a DTK.
 * 
 * The structure typedef is in match.h
 */
struct _MatchListItem {
	uint32_t day_number;
	uint8_t time_interval_number;

	MatchListItem * next;
};

/**
 * @brief The head of a match list
 *
 * This is an opaque structure that represents the head of the list. Each item
 * in the list captures a match between an RPI and a DTK.
 * 
 * This is the object usually passed as the first parameter of every non-static
 * function.
 *
 * The structure typedef is in match.h
 */
struct _MatchList {
	size_t count;
	MatchListItem * first;
	MatchListItem * last;
};

// Function prototypes

MatchListItem * match_list_item_new();
void match_list_item_delete(MatchListItem * data);
void match_list_append(MatchList * data, MatchListItem * item);

// Function definitions

/**
 * Creates a new instance of the class.
 *
 * @return The newly created object.
 */
MatchList * match_list_new() {
	MatchList * data;
	
	data = calloc(sizeof(MatchList), 1);

	return data;
}

/**
 * Deletes an instance of the class, freeing up the memory allocated to it.
 *
 * This will also delete all items contained in the list.
 *
 * @param data The instance to free.
 */
void match_list_delete(MatchList * data) {
	if (data) {
		match_list_clear(data);

		free(data);
	}
}

/**
 * Creates a new instance of the class.
 *
 * @return The newly created object.
 */
MatchListItem * match_list_item_new() {
	MatchListItem * data;
	
	data = calloc(sizeof(MatchListItem), 1);

	return data;
}

/**
 * Deletes an instance of the class, freeing up the memory allocated to it.
 *
 * @param data The instance to free.
 */
void match_list_item_delete(MatchListItem * data) {
	if (data) {
		free(data);
	}
}

/**
 * Clears all items from the list.
 *
 * Removes all items from the list to create an empty list. The memory
 * associated with the items in the list is freed.
 *
 * @param data The list to operate on.
 */
void match_list_clear(MatchList * data) {
	MatchListItem * item;
	MatchListItem * next;

	item = data->first;
	while (item) {
		next = item->next;
		match_list_item_delete(item);
		item = next;
	}
	
	data->first = NULL;
	data->last = NULL;
	data->count = 0;
}

/**
 * Returns the number of items in the list.
 *
 * Immediately after creation, or after the \ref match_list_clear() function
 * has been called, this will return zero.
 *
 * @param data The list to operate on.
 */
size_t match_list_count(MatchList * data) {
	return data->count;
}

/**
 * Returns the first item in the list.
 *
 * Useful for iterating through the items in the list.
 *
 * @param data The list to operate on.
 * @return The first item of the list.
 */
MatchListItem const * match_list_first(MatchList const * data) {
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
MatchListItem const * match_list_next(MatchListItem const * data) {
	return data->next;
}


/**
 * Returns the day number of the item in the list.
 *
 * This will represent the day number of when an interaction occurred with
 * someone who has subsequently uploaded their DTK to a diagnosis server due to
 * testing positive.
 *
 * @param data The list to operate on.
 * @return The day number for this item.
 */
uint32_t match_list_get_day_number(MatchListItem const * data) {
	return data->day_number;
}

/**
 * Returns the time interval number of the item in the list.
 *
 * This will represent the time interval number of when an interaction occurred
 * with someone who has subsequently uploaded their DTK to a diagnosis server
 * due to testing positive.
 *
 * @param data The list to operate on.
 * @return The time interval number for this item.
 */
uint8_t match_list_get_time_interval_number(MatchListItem const * data) {
	return data->time_interval_number;
}

/**
 * Adds an item to the list.
 *
 * This adds a match to the list. It's primarily for internal use.
 *
 * @param data The list to append to.
 * @param item The match to append.
 */
void match_list_append(MatchList * data, MatchListItem * item) {
	if (data->last == NULL) {
		data->first = item;
		data->last = item;
	}
	else {
		data->last->next = item;
		data->last = item;
	}
	data->count++;
}

/**
 * Returns a list of matches found between the beacons and diagnoses.
 *
 * This searches through the list of DTKs and the list of RPIs provided, and
 * returns a list of matches.
 *
 * If the returned list has any elements in, this would suggest that the user
 * has been in contact with someone who tested positive and uploaded their DTK
 * to a Diagnosis Server.
 *
 * The match list isn't cleared by this call and so any new values will be
 * appended to it.
 *
 * @param data The list that any matches will be appended to.
 * @param beacons A list of RPIs extracted from overheard BLE beacons.
 * @param diagnosis_keys A list of DTKs downloaed from a Diagnosis Server.
 */
void match_list_find_matches(MatchList * data, RpiList * beacons, DtkList * diagnosis_keys) {
	// For each diagnosis key, generate the RPIs and compare them against the captured RPI beacons
	DtkListItem const * dtk_item;
	RpiListItem const * rpi_item;
	uint8_t interval;
	bool result;
	Rpi * generated;
	MatchListItem * match;
	Dtk const * diagnosis_key;
	Rpi const * rpi;

	dtk_item = dtk_list_first(diagnosis_keys);
	generated = rpi_new();

	while (dtk_item != NULL) {
		diagnosis_key = dtk_list_get_dtk(dtk_item);
		// Generate all possible RPIs for this dtk and compare agsinst the beacons
		for (interval = 0; interval < RPI_INTERVAL_MAX; ++interval) {
			result = rpi_generate_proximity_id(generated, diagnosis_key, interval);
			if (result) {
				// Check against all beacons
				rpi_item = rpi_list_first(beacons);
				while (rpi_item != NULL) {
					rpi = rpi_list_get_rpi(rpi_item);
					result = rpi_compare(rpi, generated);

					if (result) {
						if (interval != rpi_get_time_interval_number(rpi)) {
							result = false;
							LOG(LOG_DEBUG, "Matched beacons don't match intervals\n");
						}
					}
					
					if (result) {
						match = match_list_item_new();
						match->day_number = dtk_get_day_number(diagnosis_key);
						match->time_interval_number = interval;
						match_list_append(data, match);
					}

					rpi_item = rpi_list_next(rpi_item);
				}
			}
		}

		dtk_item = dtk_list_next(dtk_item);
	}

	rpi_delete(generated);
}

/** @} addtogroup Matching*/

