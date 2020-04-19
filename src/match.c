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

struct _MatchListItem {
	uint32_t day_number;
	uint8_t time_interval_number;

	MatchListItem * next;
};

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

MatchList * match_list_new() {
	MatchList * data;
	
	data = calloc(sizeof(MatchList), 1);

	return data;
}

void match_list_delete(MatchList * data) {
	if (data) {
		match_list_clear(data);

		free(data);
	}
}

MatchListItem * match_list_item_new() {
	MatchListItem * data;
	
	data = calloc(sizeof(MatchListItem), 1);

	return data;
}

void match_list_item_delete(MatchListItem * data) {
	if (data) {
		free(data);
	}
}

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

size_t match_list_count(MatchList * data) {
	return data->count;
}

MatchListItem const * match_list_first(MatchList const * data) {
	return data->first;
}

MatchListItem const * match_list_next(MatchListItem const * data) {
	return data->next;
}


uint32_t match_list_get_day_number(MatchListItem const * data) {
	return data->day_number;
}

uint8_t match_list_get_time_interval_number(MatchListItem const * data) {
	return data->time_interval_number;
}

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


