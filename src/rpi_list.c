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

#include "contrac/contrac.h"
#include "contrac/utils.h"
#include "contrac/log.h"
#include "contrac/dtk_list.h"

#include "contrac/rpi_list.h"

// Defines

// Structures

struct _RpiListItem {
	Rpi * rpi;
	RpiListItem * next;
};

struct _RpiList {
	RpiListItem * first;
	RpiListItem * last;
};

// Function prototypes

// Function definitions

RpiList * rpi_list_new() {
	RpiList * data;
	
	data = calloc(sizeof(RpiList), 1);

	return data;
}

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

RpiListItem const * rpi_list_first(RpiList const * data) {
	return data->first;
}

RpiListItem const * rpi_list_next(RpiListItem const * data) {
	return data->next;
}

Rpi const * rpi_list_get_rpi(RpiListItem const * data) {
	return data->rpi;
}

void rpi_list_add_beacon(RpiList * data, unsigned char const * rpi_bytes, uint8_t time_interval_number) {
	Rpi * rpi = rpi_new();
	rpi_assign(rpi, rpi_bytes, time_interval_number);
	rpi_list_append(data, rpi);
}




