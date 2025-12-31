//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include <stdlib.h>
#include "apply.h"
#include "../BANK_fonts/get.h"
#include "../BANK_fonts/library.h"
#include "../BANK_fonts/picker.h"
#include "../BANK_fonts/set.h"
#include "../BANK_spui/bool_invalid.h"
#include "../BANK_spui/font_invalid.h"
#include "../BANK_spui/font_not_found.h"
#include "../BANK_spui/not_zero.h"
#include "../BANK_spui/unknown_setting.h"
#include "../BANK_system/palette_apply.h"
#include "../BANK_system/palettes.h"
#include "../common/memory.h"
#include "../liblayer3/liblayer3.h"
#include "ini_common.h"
#include "show_errors.h"
#include "string_is_false.h"
#include "string_is_true.h"
#include "trim_whitespace.h"
#include <arch/zxn.h>
#include <stdio.h>
#include <string.h>

int16_t SettingsTempValue;
void settings_apply(char *Command) {
	// not all settings are KV pairs
	IniKey = 0;
	IniKey = trim_whitespace(strtok(Command, "="));
	if(IniKey[strlen(IniKey)-1]=='\n') {
		IniKey[strlen(IniKey)-1] = 0;
		IniValue = 0;
	}
	else {
		IniValue = trim_whitespace(strtok(NULL, "\n"));
	}

	if(!stricmp(IniKey, "errors")) {
		/*
		 * Error Supression
		 */
		if(string_is_false(IniValue)) {
			SettingsShowErrors = false;
			return;
		}
		else if(string_is_true(IniValue)) {
			SettingsShowErrors = true;
			return;
		}
		// Load Font from file
		else if (SettingsShowErrors) {
			_farWithPointer(BANK_SPUI, (void *(*)(void *))spui_bool_invalid, (void *)IniKey);
		}
		return;
	} else
	if(!stricmp(IniKey, "font")) {
		/*
		 * Font
		 */
		// Use bundled font
		if(IniValue) {
			if (IniValue[0] != '/') {
				SettingsTempValue = _farWithPointer(BANK_FONTS, font_get, IniValue);
				// -1 means we did not find a font
				if (SettingsTempValue < FONT_COUNT) {
					_farWithUChar(BANK_FONTS, font_set, SettingsTempValue);
					return;
				} else {
					// ERROR
					_farWithPointer(BANK_SPUI, (void *(*)(void *))spui_font_invalid, IniValue);
					return;
				}

			} else if (SettingsShowErrors) {
				_farWithPointer(BANK_SPUI, (void *(*)(void *))spui_font_not_found, IniValue);
			}
		} else {
			_far(BANK_FONTS, (void *)font_picker);
		}
		return;
	} else
	if(!stricmp(IniKey, "repeat_start")) {
		/*
		 * Key Initial Repeat
		 */
		SettingsTempValue = (atoi)(IniValue);
		if(SettingsTempValue) {
			L3RepeatStart = SettingsTempValue;
		}
		else if (SettingsShowErrors) {
			_far(BANK_SPUI, (void *)spui_not_zero);
		}
		return;
	} else
	if(!stricmp(IniKey, "repeat_key")) {
		/*
		 * Key Subsiquent Repeat
		 */
		SettingsTempValue = (atoi)(IniValue);
		if(SettingsTempValue) {
			L3RepeatKey = SettingsTempValue;
		}
		else if (SettingsShowErrors) {
			_far(BANK_SPUI, (void *)spui_not_zero);
		}
		return;
	} else
	if(!stricmp(IniKey, "cursor_flash")) {
		/*
		 * Cursor Flash Rate
		 */
		SettingsTempValue = (atoi)(IniValue);
		if(SettingsTempValue) {
			L3CursorFlashRate = SettingsTempValue;
		}
		else if (SettingsShowErrors) {
			_far(BANK_SPUI, spui_not_zero);
		}
		return;
	} else
	if(!stricmp(IniKey, "colour_default_background")) {
		/*
		 * Colour: Default / Background
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_DEFAULT_BACKGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_default_foreground")) {
		/*
		 * Colour: Default / Foreground
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_DEFAULT_FOREGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_cursor_background")) {
		/*
		 * Colour: Cursor / Background
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_CURSOR_BACKGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_cursor_foreground")) {
		/*
		 * Colour: Cursor / Foreground
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_CURSOR_FOREGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_status_background")) {
		/*
		 * Colour: Status / Background
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_STATUS_BACKGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_status_foreground")) {
		/*
		 * Colour: Status / Foreground
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_STATUS_FOREGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_error_background")) {
		/*
		 * Colour: Error / Background
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_ERROR_BACKGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if(!stricmp(IniKey, "colour_error_foreground")) {
		/*
		 * Colour: Error / Foreground
		 */
		SettingsTempValue = (atoi)(IniValue);
		Command[0] = SYSTEM_PALETTE_ERROR_FOREGROUND;
		Command[1] = (SettingsTempValue) & 255;
		Command[2] = (SettingsTempValue>>8) & 1;

		_farWithPointer(BANK_SYSTEM, palette_apply, Command);

		return;
	} else
	if (SettingsShowErrors) {
		_farWithPointer(BANK_SPUI, spui_unknown_setting, IniKey);
	}
	return;
}
