/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the .
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */

#include "liblayer3.h"

uint16_t screencolour = TEXTMODE_DEFAULT_COLOUR;

void con_putc(uint16_t c)
{
    if (screeny >= SCREENHEIGHT)
            return;

    if (c < 32)
    {
            con_putc('^');
            c += '@';
    }

    tilemap[screeny][screenx].tile = c;
    tilemap[screeny][screenx].flags = screencolour;
    screenx++;
    if (screenx == SCREENWIDTH)
    {
            screenx = 0;
            screeny++;
    }
}

