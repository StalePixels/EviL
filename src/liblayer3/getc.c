/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */
#include <z80.h>
#include <arch/zxn.h>
#include <intrinsic.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include "liblayer3.h"
#include <input.h>
#include <stdbool.h>

static uint8_t last_key = 0;
static uint8_t next_key = 0;

uint8_t L3RepeatStart = LIBLAYER3_REPEAT_START;
uint8_t L3RepeatKey = LIBLAYER3_REPEAT_KEY;

static uint8_t repeat_counter;
static bool	key_repeating = false;


uint8_t L3CursorFlashRate = LIBLAYER3_CURSOR_FLASH_RATE;

void l3_cursor() {
	L3CursorFlashRate--;
    WAIT_FOR_SCANLINE(239);
    if(L3CursorFlashRate ==0) {
        tilemap[L3ScreenY][L3ScreenX].flags = tilemap[L3ScreenY][L3ScreenX].flags + 128;
		L3CursorFlashRate = LIBLAYER3_CURSOR_FLASH_RATE;
    }
}

int8_t chricmp(unsigned char a, unsigned char b) {
    if((a>64 && a<91)) a=a+32;
    if((b>64 && b<91)) b=b+32;
    return a==b;
}

uint8_t l3_getc(void)
{
    // Stop holding the previous key
    while(chricmp(next_key, last_key)) {
        next_key = in_inkey();

		if(!next_key) goto ready_next_key;

        l3_cursor();
		repeat_counter--;
		if(!repeat_counter) {
			// pretend they raised their finger for a moment, and then put it back
			next_key = 0;
			key_repeating = true;
			repeat_counter = L3RepeatKey;

			goto get_next_key;
		}
    }

ready_next_key:
	key_repeating = false;
	repeat_counter = L3RepeatStart;
get_next_key:

    // Now wait for a new key
    while(!next_key) {
		// And if we faked a finger_raise, this will act like it's put down again
        next_key = in_inkey();
        l3_cursor();
    }

    // Remember they key so we don't repeat it
    last_key = next_key;

    tilemap[L3ScreenY][L3ScreenX].flags = L3ScreenColour;
	L3CursorFlashRate = LIBLAYER3_CURSOR_FLASH_RATE;
	return next_key;
}


