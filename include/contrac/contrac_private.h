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

#ifndef __CONTRAC_PRIVATE_H
#define __CONTRAC_PRIVATE_H

// Includes

#include "contrac/contrac.h"

// Defines

// Data sizes in bytes

// Structures

// Function prototypes

void contrac_set_tracing_key(Contrac * data, unsigned char const * tracing_key);
bool contrac_set_tracing_key_base64(Contrac * data, char const * tracing_key);

// Function definitions

#endif // __CONTRAC_PRIVATE_H

