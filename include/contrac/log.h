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

#ifndef __LOG_H
#define __LOG_H

// Includes

#include <syslog.h>

// Defines

#define LOG(level, ...) log_priority(level, __VA_ARGS__);

// Structures

// Function prototypes

void log_priority(int priority, const char *format, ...);

// Function definitions

#endif // __LOG_H
