/* libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 */

#include "libcuss.h"

void con_revon(void)
{
    screencolour = TEXTMODE_REVERSE_COLOUR;
	#if defined LIBCUSS_REVON
		cpm_printstring0(LIBCUSS_REVON);
	#endif
}


