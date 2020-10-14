/* libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 */

#include <stdio.h>
#include "libcuss.h"

void con_goto(uint16_t x, uint16_t y)
{
	screenx = x;
	screeny = y;
}

