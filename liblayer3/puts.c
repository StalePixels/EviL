/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */

#include "liblayer3.h"

void con_puts(const char* s)
{
	for (;;)
	{
		uint16_t c = *s++;
		if (!c)
			break;
		con_putc(c);
	}
}

