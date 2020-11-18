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

#define L3_REPEAT_START         25
static uint8_t repeat_start = L3_REPEAT_START;
#define L3_REPEAT_KEY           15
static uint8_t repeat_key = L3_REPEAT_KEY;

static uint8_t repeat_counter;
static bool	key_repeating = false;


#define FRAME_FLASH             10
static uint8_t cursor_frame_counter =    FRAME_FLASH;

void l3_cursor() {
    cursor_frame_counter--;
    WAIT_FOR_SCANLINE(239);
    if(cursor_frame_counter==0) {
        tilemap[ScreenY][ScreenX].flags = tilemap[ScreenY][ScreenX].flags + 128;
        cursor_frame_counter = FRAME_FLASH;
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
        l3_cursor();
		repeat_counter--;
		if(!repeat_counter) {
			// pretend they raised their finger for a moment, and then put it back
			next_key = 0;

			if(!key_repeating) {
				key_repeating = true;
				repeat_counter = repeat_start;
			}
			else {
				repeat_counter = repeat_key;
			}
			goto get_next_key;
		}
    }

	repeat_counter = repeat_start;
get_next_key:

    // Now wait for a new key
    while(!next_key) {
        next_key = in_inkey();
        l3_cursor();
    }

    // Remember they key so we don't repeat it
    last_key = next_key;

    tilemap[ScreenY][ScreenX].flags = tilemap[ScreenY][ScreenX].flags = screencolour;
	return next_key;
}


