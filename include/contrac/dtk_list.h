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

#ifndef __DTK_LIST_H
#define __DTK_LIST_H

// Includes

#include "contrac/contrac.h"
#include "contrac/dtk.h"

// Defines

// Structures

/**
 * An opaque structure that represents the head of the list.
 * 
 * The internal structure can be found in dtk_list.c
 */
typedef struct _DtkList DtkList;

/**
 * An opaque structure that represents an item in the list.
 * 
 * The internal structure can be found in dtk_list.c
 */
typedef struct _DtkListItem DtkListItem;

// Function prototypes

DtkList * dtk_list_new();
void dtk_list_delete(DtkList * data);

void dtk_list_append(DtkList * data, Dtk * dtk);
void dtk_list_add_diagnosis(DtkList * data, unsigned char const * dtk_bytes, uint32_t day_number);

DtkListItem const * dtk_list_first(DtkList const * data);
DtkListItem const * dtk_list_next(DtkListItem const * data);
Dtk const * dtk_list_get_dtk(DtkListItem const * data);

// Function definitions

#endif // __DTK_LIST_H

/** @} addtogroup Containers*/

