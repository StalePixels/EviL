/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the .
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */
#include <stdio.h>
#include "liblayer3.h"

void con_goto(uint16_t x, uint16_t y)
{
	screenx = x;
	screeny = y;
}

