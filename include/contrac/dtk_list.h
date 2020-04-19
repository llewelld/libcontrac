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

#ifndef __DTK_LIST_H
#define __DTK_LIST_H

// Includes

#include "contrac/contrac.h"
#include "contrac/dtk.h"

// Defines

// Structures

typedef struct _DtkList DtkList;
typedef struct _DtkListItem DtkListItem;

// Function prototypes

DtkList * dtk_list_new();
void dtk_list_delete(DtkList * data);

void dtk_list_append(DtkList * data, Dtk * dtk);
DtkListItem const * dtk_list_first(DtkList const * data);
DtkListItem const * dtk_list_next(DtkListItem const * data);
Dtk const * dtk_list_get_dtk(DtkListItem const * data);

// Function definitions

#endif // __DTK_LIST_H


