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
 * @brief Provides a way to match collected RPIs with downloaded DTKs
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

#ifndef __MATCH_H
#define __MATCH_H

// Includes

#include "contrac/contrac.h"
#include "contrac/dtk.h"

// Defines

// Structures

/**
 * An opaque structure that represents the head of the list.
 * 
 * The internal structure can be found in match.c
 */
typedef struct _MatchList MatchList;

/**
 * An opaque structure that represents an item in the list.
 * 
 * The internal structure can be found in match.c
 */
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

/** @} addtogroup Matching*/

