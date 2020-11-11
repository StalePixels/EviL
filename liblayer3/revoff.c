/* libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 */

#include "liblayer3.h"

void con_revoff(void)
{
    screencolour = TEXTMODE_DEFAULT_COLOUR;
	#if defined LIBLAYER3_REVOFF
		cpm_printstring0(LIBLAYER3_REVOFF);
	#endif
}



