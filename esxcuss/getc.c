/* libcuss © 2020 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 */

#include <z80.h>
#include <arch/zxn.h>
#include <intrinsic.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include <input.h>
#include "libcuss.h"

static uint8_t last_key = 0;
static uint8_t next_key = 0;

#define FRAME_FLASH             27
static uint8_t cursor_frame_counter =    FRAME_FLASH;

void con_cursor() {
    cursor_frame_counter--;
    WAIT_FOR_SCANLINE(239);
    if(cursor_frame_counter==0) {
        tilemap[screeny][screenx].flags = tilemap[screeny][screenx].flags + 128;
        cursor_frame_counter = FRAME_FLASH;
    }
}

int8_t chrcmpi(unsigned char a, unsigned char b) {
    if((a>64 && a<91)) a=a+32;
    if((b>64 && b<91)) b=b+32;
    return a==b;
}

uint8_t con_getc(void)
{
    // Stop holding the previous key
    while(chrcmpi(next_key, last_key)) {
        next_key = in_inkey();
        con_cursor();
    }

    // Now wait for a new key
    while(!next_key) {
        next_key = in_inkey();
        con_cursor();
    }

    // Remember they key so we don't repeat it
    last_key = next_key;

    tilemap[screeny][screenx].flags = tilemap[screeny][screenx].flags = screencolour;
	return next_key;
}


