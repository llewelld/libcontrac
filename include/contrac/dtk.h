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

#ifndef __DTK_H
#define __DTK_H

// Includes

// Defines

#define DTK_SIZE (16)
#define DTK_SIZE_BASE64 (24)

// Structures

typedef struct _Dtk Dtk;

// Function prototypes

Dtk * dtk_new();
void dtk_delete(Dtk * data);

bool contrac_generate_daily_key(Dtk * data, Contrac const * contrac, uint32_t day_number);
const unsigned char * dtk_get_daily_key(Dtk const * data);
uint32_t dtk_get_day_number(Dtk const * data);
void dtk_assign(Dtk * data, unsigned char const * dtk_bytes, uint32_t day_number);

// Function definitions

#endif // __DTK_H


