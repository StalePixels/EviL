/* libcuss © 2019 David Given
 * This library is distributable under the terms of the 2-clause BSD license.
 * See COPYING.cpmish in the distribution root directory for more information.
 */


#ifndef LIBCUSS_H
#define LIBCUSS_H

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
#define LIBCUSS_ANSI

// OCTAL BATSHITTERY AHEAD!
#define LIBCUSS_CLEAR "\033[2J"
#define LIBCUSS_CLEAREOL "\033[K"
#define LIBCUSS_NEWLINE "\r\n"
//#define LIBCUSS_REVON "\033[7m"
//#define LIBCUSS_REVOFF "\033[0m"

#define LIBCUSS_KEY_LEFT "\010"
#define LIBCUSS_KEY_DOWN "\012"
#define LIBCUSS_KEY_UP "\013"
//#define LIBCUSS_KEY_RIGHT "\014"
#define LIBCUSS_KEY_RIGHT "\011"

#endif // #LIBCUSS_H
