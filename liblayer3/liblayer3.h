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
#include "../evil_banked.h"
#include "textmode.h"

extern uint16_t screenx;
extern uint16_t screeny;
extern uint16_t screencolour;

extern void con_goto(uint16_t x, uint16_t y);
extern void con_clear(void);
extern void con_putc(uint16_t c);
extern void con_puts(const char* s);
extern uint8_t con_getc(void);
extern void con_newline(void);
extern void con_clear_to_eol(void);
extern void con_revon(void);
extern void con_revoff(void);

#define SCREENWIDTH 80
#define SCREENHEIGHT 32
#define LIBLAYER3_ANSI

// OCTAL BATSHITTERY AHEAD!
#define LIBLAYER3_CLEAR "\033[2J"
#define LIBLAYER3_CLEAREOL "\033[K"
#define LIBLAYER3_NEWLINE "\r\n"
//#define LIBLAYER3_REVON "\033[7m"
//#define LIBLAYER3_REVOFF "\033[0m"

#define LIBLAYER3_KEY_LEFT "\010"
#define LIBLAYER3_KEY_DOWN "\012"
#define LIBLAYER3_KEY_UP "\013"
//#define LIBLAYER3_KEY_RIGHT "\014"
#define LIBLAYER3_KEY_RIGHT "\011"

#endif // #LIBLAYER3_H
