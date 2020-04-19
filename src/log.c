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

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>

#include "contrac/log.h"

// Defines

// Structures

// Function prototypes

void log_priority(int priority, const char *format, ...) {
	va_list args;
	int length;
	char *buffer;

	va_start(args, format);
	length = vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	buffer = malloc(sizeof(char) * (length + 1));

	va_start(args, format);
	length = vsnprintf(buffer, length + 1, format, args);
	va_end(args);
	buffer[length] = 0;
	
	syslog(priority, "%s", buffer);
}

// Function definitions

