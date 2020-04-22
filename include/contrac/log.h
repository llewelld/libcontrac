/** \ingroup Logging
 * @file
 * @author	David Llewellyn-Jones <david@flypig.co.uk>
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 * Copyright David Llewellyn-Jones, 2020
 * Released under the GPLv2.
 *
 * @brief Allows output to be sent to the log
 * @section DESCRIPTION
 *
 * This is a simple set of functions and macros that allows strings to be
 * recorded in the syslog.
 *
 */

/** \addtogroup Logging
 *  @{
 */

#ifndef __LOG_H
#define __LOG_H

// Includes

#include <syslog.h>

// Defines

/**
 * Log a string to syslog.
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
#define LOG(level, ...) log_priority(level, __VA_ARGS__);

// Structures

// Function prototypes

void log_priority(int priority, const char *format, ...);

// Function definitions

#endif // __LOG_H

/** @} addtogroup Logging*/
