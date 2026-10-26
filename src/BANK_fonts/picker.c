//
// Created by D Rimron-Soutter on 22/11/2020.
//
// The font picker: :set font with no value. It lists the bundled fonts in the help window,
// and the screen changes to the chosen font at once. ENTER keeps it, BREAK puts the old font back.
//

#include "picker.h"

#include <stdint.h>
#include <string.h>

#include "../BANK_spui/help.h"
#include "../common/help.h"
#include "../common/memory.h"
#include "../liblayer3/liblayer3.h"
#include "library.h"
#include "set.h"

// The names are in 3 columns of 6, in the order of font_names
#define FONT_PICKER_ROWS		6
#define FONT_PICKER_WIDTH		19
// in_inkey's codes for the cursor keys
#define FONT_PICKER_LEFT		0x08
#define FONT_PICKER_RIGHT		0x09
#define FONT_PICKER_DOWN		0x0A
#define FONT_PICKER_UP			0x0B

static void font_picker_names(uint8_t choice) {
	struct __tilemap *cell;
	const char *name;
	uint8_t font_id, attr;

	for(font_id = 0; font_id != FONT_FACES; ++font_id) {
		cell = &tilemap[HelpTop + SPUI_HELP_TEXT_ROW + 1 + (font_id % FONT_PICKER_ROWS)]
				[SPUI_HELP_TEXT_COL + (font_id / FONT_PICKER_ROWS) * FONT_PICKER_WIDTH];
		attr = (font_id == choice) ? SPUI_HELP_KEY_ATTR : SPUI_HELP_ATTR;
		cell->tile = (font_id == choice) ? '>' : ' ';
		cell->flags = attr;
		for(name = font_names[font_id]; *name; ++name) {
			++cell;
			cell->tile = *name;
			cell->flags = attr;
		}
	}
}

void font_picker(void) {
	uint8_t help_top = HelpTop;
	uint8_t old_font = FontCurrent;
	uint8_t choice = FONT_NONE;
	uint8_t font_id, c;

	memcpy(FontBackup, FONT_DEFINITIONS, FONT_SIZE);

	// An alias marks the font it names
	if(old_font != FONT_NONE) {
		for(font_id = 0; font_id != FONT_FACES; ++font_id) {
			if(font_library[font_id] == font_library[old_font]) {
				choice = font_id;
				break;
			}
		}
	}

	// The picker takes the place of the help window, at the top: the cursor is on the command row.
	// The help draws itself again once the command is done.
	HelpTop = 0;
	L3Modal = true;
	_far(BANK_SPUI, (FarFn)spui_font_picker);

	for(;;) {
		font_picker_names(choice);
		c = l3_getc();

		if(c == 13)
			break;

		if(c == LIBLAYER3_KEY_BREAK) {
			memcpy(FONT_DEFINITIONS, FontBackup, FONT_SIZE);
			FontCurrent = old_font;
			break;
		}

		if(c == FONT_PICKER_DOWN || c == 'j') {
			// From FONT_NONE (255), DOWN goes to the first font
			if(++choice >= FONT_FACES)
				choice = 0;
		}
		else if(c == FONT_PICKER_UP || c == 'k') {
			if(!choice || choice >= FONT_FACES)
				choice = FONT_FACES;
			--choice;
		}
		// LEFT and RIGHT move a whole column, round to the other side
		else if(c == FONT_PICKER_RIGHT || c == 'l') {
			if(choice >= FONT_FACES)
				choice = 0;
			else if((choice += FONT_PICKER_ROWS) >= FONT_FACES)
				choice -= FONT_FACES;
		}
		else if(c == FONT_PICKER_LEFT || c == 'h') {
			if(choice >= FONT_FACES)
				choice = 0;
			else if(choice < FONT_PICKER_ROWS)
				choice += FONT_FACES - FONT_PICKER_ROWS;
			else
				choice -= FONT_PICKER_ROWS;
		}
		else
			continue;

		font_set(choice);
	}

	L3Modal = false;
	_far(BANK_SPUI, (FarFn)spui_help_erase);
	HelpTop = help_top;
}
