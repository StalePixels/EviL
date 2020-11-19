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

void l3_goto(uint16_t x, uint16_t y)
{
	L3ScreenX = x;
	L3ScreenY = y;
}

