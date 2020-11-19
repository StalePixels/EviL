/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */

#include "liblayer3.h"

uint16_t L3ScreenColour = TEXTMODE_DEFAULT_COLOUR;

void l3_putc(uint16_t c)
{
    if (L3ScreenY >= LIBLAYER3_SCREENHEIGHT)
            return;

    if (c < 32)
    {
            l3_putc('^');
            c += '@';
    }

    tilemap[L3ScreenY][L3ScreenX].tile = c;
    tilemap[L3ScreenY][L3ScreenX].flags = L3ScreenColour;
	L3ScreenX++;
    if (L3ScreenX == LIBLAYER3_SCREENWIDTH)
    {
		L3ScreenX = 0;
		L3ScreenY++;
    }
}

