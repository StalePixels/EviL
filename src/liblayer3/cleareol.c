/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */
#include <stdio.h>
#include "liblayer3.h"

void l3_clear_to_eol(void)
{
    if (L3ScreenY >= LIBLAYER3_SCREENHEIGHT)
            return;

    uint16_t i = LIBLAYER3_SCREENWIDTH - L3ScreenX;
    if ((i != 0) && (L3ScreenY == (LIBLAYER3_SCREENHEIGHT -1)))
        i--;

    while (i--) {
        tilemap[L3ScreenY][LIBLAYER3_SCREENWIDTH -i-1].tile = ' ';
        tilemap[L3ScreenY][LIBLAYER3_SCREENWIDTH -i-1].flags = L3ScreenColour;
    }

    l3_goto(L3ScreenX, L3ScreenY);
}

