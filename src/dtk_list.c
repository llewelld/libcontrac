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

// Defines

// Structures

struct _DtkListItem {
	Dtk * dtk;
	DtkListItem * next;
};

struct _DtkList {
	DtkListItem * first;
	DtkListItem * last;
};

// Function prototypes

// Function definitions

DtkList * dtk_list_new() {
	DtkList * data;
	
	data = calloc(sizeof(DtkList), 1);

	return data;
}

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

DtkListItem const * dtk_list_first(DtkList const * data) {
	return data->first;
}

DtkListItem const * dtk_list_next(DtkListItem const * data) {
	return data->next;
}

Dtk const * dtk_list_get_dtk(DtkListItem const * data) {
	return data->dtk;
}


