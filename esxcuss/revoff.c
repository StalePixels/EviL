/* libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 */

#include "libcuss.h"

void con_revoff(void)
{
    screencolour = TEXTMODE_DEFAULT_COLOUR;
	#if defined LIBCUSS_REVOFF
		cpm_printstring0(LIBCUSS_REVOFF);
	#endif
}



