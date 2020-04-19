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

#ifndef __MATCH_H
#define __MATCH_H

// Includes

#include "contrac/contrac.h"
#include "contrac/dtk.h"

// Defines

// Structures

typedef struct _MatchList MatchList;
typedef struct _MatchListItem MatchListItem;

// Function prototypes

MatchList * match_list_new();
void match_list_delete(MatchList * data);

void match_list_clear(MatchList * data);
size_t match_list_count(MatchList * data);

uint32_t match_list_get_day_number(MatchListItem const * data);
uint8_t match_list_get_time_interval_number(MatchListItem const * data);

MatchListItem const * match_list_first(MatchList const * data);
MatchListItem const * match_list_next(MatchListItem const * data);

void match_list_find_matches(MatchList * data, RpiList * beacons, DtkList * diagnosis_keys);

// Function definitions

#endif // __MATCH_H


