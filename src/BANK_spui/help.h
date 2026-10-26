//
// The help screen: BREAK shows the keys of the current mode
//

#ifndef EVIL_SPUI_HELP_H
#define EVIL_SPUI_HELP_H

#include <stdint.h>
#include "../liblayer3/textmode.h"

// ULA cells: column x is tilemap columns 8 + 2x and 9 + 2x, row y is tilemap row 4 + y
#define SPUI_HELP_LEFT			1
#define SPUI_HELP_RIGHT			30
#define SPUI_HELP_WIDTH			(SPUI_HELP_RIGHT - SPUI_HELP_LEFT + 1)
#define SPUI_HELP_TILE_ROW		4
#define SPUI_HELP_TILE_COL		(8 + 2 * SPUI_HELP_LEFT)
#define SPUI_HELP_COLUMNS		(2 * SPUI_HELP_WIDTH)
#define SPUI_HELP_TEXT_COL		(SPUI_HELP_TILE_COL + 2)
#define SPUI_HELP_TEXT_WIDTH	(SPUI_HELP_COLUMNS - 4)
#define SPUI_HELP_LINES			9
// The tilemap row of the first text line, below the title bar
#define SPUI_HELP_TEXT_ROW		(SPUI_HELP_TILE_ROW + 1)


// Tilemap palette entries 6 and 7 are black on white, 8 and 9 red on white (BANK_system/palettes.c)
#define SPUI_HELP_ATTR			(6 | TEXTMODE_ULA_FRONT)
#define SPUI_HELP_KEY_ATTR		(8 | TEXTMODE_ULA_FRONT)

void spui_help(uint8_t Mode);
void spui_help_erase(void);
// Draws the font picker's window, without the font names, where HelpTop says
void spui_font_picker(void);

#endif//EVIL_SPUI_HELP_H
