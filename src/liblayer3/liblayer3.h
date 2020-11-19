/*
 * liblayer3 for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   original libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */
#ifndef LIBLAYER3_H
#define LIBLAYER3_H

#include <stdint.h>
#include "../BANK_system/system.h"
#include "textmode.h"


extern uint8_t L3ScreenX, L3ScreenY;
extern uint16_t L3ScreenColour;
extern uint8_t L3CursorFlashRate;

#define LIBLAYER3_REPEAT_START 25
#define LIBLAYER3_REPEAT_KEY 15
#define LIBLAYER3_CURSOR_FLASH_RATE 10

extern uint8_t L3RepeatStart;
extern uint8_t L3RepeatKey;

extern void l3_goto(uint16_t x, uint16_t y);
extern void l3_clear(void);
extern void l3_putc(uint16_t c);
extern void l3_puts(const char* s);
extern uint8_t l3_getc(void);
extern void l3_newline(void);
extern void l3_clear_to_eol(void);
extern void l3_revon(void);
extern void l3_revoff(void);

#define LIBLAYER3_SCREENWIDTH 80
#define LIBLAYER3_SCREENHEIGHT 32
#define LIBLAYER3_ANSI

// OCTAL BATSHITTERY AHEAD!
#define LIBLAYER3_CLEAR "\033[2J"
#define LIBLAYER3_CLEAREOL "\033[K"
#define LIBLAYER3_NEWLINE "\r\n"

#define LIBLAYER3_KEY_LEFT "\010"
#define LIBLAYER3_KEY_DOWN "\012"
#define LIBLAYER3_KEY_UP "\013"
//#define LIBLAYER3_KEY_RIGHT "\014"
#define LIBLAYER3_KEY_RIGHT "\011"

#endif // #LIBLAYER3_H
