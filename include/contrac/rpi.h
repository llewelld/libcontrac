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

#ifndef __RPI_H
#define __RPI_H

// Includes

#include "contrac/contrac.h"
#include "contrac/dtk.h"

// Defines

#define RPI_SIZE (16)
#define RPI_SIZE_BASE64 (24)
#define RPI_INTERVAL_MAX (144)

// Structures

typedef struct _Rpi Rpi;

// Function prototypes

Rpi * rpi_new();
void rpi_delete(Rpi * data);

bool rpi_generate_proximity_id(Rpi * data, Dtk const * dtk, uint8_t time_interval_number);
const unsigned char * rpi_get_proximity_id(Rpi const * data);
uint8_t rpi_get_time_interval_number(Rpi const * data);
void rpi_assign(Rpi * data, unsigned char const * rpi_bytes, uint8_t time_interval_number);
bool rpi_compare(Rpi const * data, Rpi const * comparitor);

// Function definitions

#endif // __RPI_H


