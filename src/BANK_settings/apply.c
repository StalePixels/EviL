//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "apply.h"
#include "../BANK_fonts/get.h"
#include "../BANK_fonts/set.h"
#include "../BANK_spui/font_invalid.h"
#include "../BANK_spui/font_not_found.h"
#include "../common/memory.h"
#include "ini_common.h"
#include "trim_whitespace.h"
#include <stdio.h>
#include <string.h>

void settings_apply(const char *command) {
	ini_key = trim_whitespace(strtok(command, "="));
	ini_value = trim_whitespace(strtok(NULL, "\n"));

	if(!stricmp(ini_key, "font")) {
		// Use bundled font
		if(ini_value[0] != '/') {
			int8_t font_id = _farWithPointer(BANK_FONTS, font_get, ini_value);
			// -1 means we did not find a font
			if(font_id!=-1) {
				_farWithUChar(BANK_FONTS, font_set, font_id);
			}
			else {
				// ERROR
				_farWithPointer(BANK_SPUI, spui_font_invalid, ini_value);
			}

		}
		// Load Font from file
		else {
			_farWithPointer(BANK_SPUI, spui_font_not_found, ini_value);

		}

	}
}