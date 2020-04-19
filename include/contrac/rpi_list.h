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

#ifndef __RPI_LIST_H
#define __RPI_LIST_H

// Includes

#include "contrac/contrac.h"
#include "contrac/rpi.h"

// Defines

// Structures

typedef struct _RpiList RpiList;
typedef struct _RpiListItem RpiListItem;

// Function prototypes

RpiList * rpi_list_new();
void rpi_list_delete(RpiList * data);

void rpi_list_append(RpiList * data, Rpi * rpi);

RpiListItem const * rpi_list_first(RpiList const * data);
RpiListItem const * rpi_list_next(RpiListItem const * data);
Rpi const * rpi_list_get_rpi(RpiListItem const * data);

// Function definitions

#endif // __RPI_LIST_H


