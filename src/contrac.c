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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <openssl/crypto.h>

#include "contrac/contrac.h"

// Defines

// Structures

struct _Contrac {
	int version;
};

// Function prototypes

// Function definitions

Contrac * contrac_new() {
	Contrac * data;
	
	data = calloc(sizeof(Contrac), 1);
	data->version = 0;
	
	return data;
}

void contrac_delete(Contrac * data) {
	if (data) {
		free(data);
	}
}


