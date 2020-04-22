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

/**
 * Log a string to syslog.
 *
 * In generl, the LOG macro should be used instead.
 *
 * Constructs a message using the supplied format and parameters and records it
 * in the system log. The format is the same as for printf.
 *
 * The logging levels are the standard syslog levels:
 *
 * LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERR, etc.
 *
 * @param priority The log level priority.
 * @param format The format for the message, the same as for printf.
 * @param ... parameters to combine with the format to create the message.
 */
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

