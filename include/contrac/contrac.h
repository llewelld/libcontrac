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

#ifndef __CONTRAC_H
#define __CONTRAC_H

// Includes

#include <stdint.h>
#include <stdbool.h>

// Defines

// Data sizes in bytes
#define TK_SIZE (32)
#define TK_SIZE_BASE64 (44)

// Structures

typedef struct _Contrac Contrac;

// Function prototypes

Contrac * contrac_new();
void contrac_delete(Contrac * data);

bool contrac_generate_tracing_key(Contrac * data);
bool contrac_set_day_number(Contrac * data, uint32_t day_number);
bool contrac_set_time_interval_number(Contrac * data, uint8_t time_interval_number);
bool contrac_update_current_time(Contrac * data);
bool contrac_get_initialised(Contrac const * data);

const unsigned char * contrac_get_tracing_key(Contrac const * data);
void contrac_get_tracing_key_base64(Contrac const * data, char * base64);

void contrac_set_tracing_key(Contrac * data, unsigned char const * tracing_key);
bool contrac_set_tracing_key_base64(Contrac * data, char const * tracing_key);

const unsigned char * contrac_get_daily_key(Contrac const * data);
void contrac_get_daily_key_base64(Contrac const * data, char * base64);

const unsigned char * contrac_get_proximity_id(Contrac const * data);
void contrac_get_proximity_id_base64(Contrac const * data, char * base64);

// Function definitions

#endif // __CONTRAC_H

