/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */

#include "liblayer3.h"

uint16_t screencolour = TEXTMODE_DEFAULT_COLOUR;

void l3_putc(uint16_t c)
{
    if (ScreenY >= SCREENHEIGHT)
            return;

    if (c < 32)
    {
            l3_putc('^');
            c += '@';
    }

    tilemap[ScreenY][ScreenX].tile = c;
    tilemap[ScreenY][ScreenX].flags = screencolour;
	ScreenX++;
    if (ScreenX == SCREENWIDTH)
    {
		ScreenX = 0;
		ScreenY++;
    }
}

