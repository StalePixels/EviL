//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "apply.h"
#include "../BANK_fonts/get.h"
#include "../BANK_fonts/set.h"
#include "../BANK_spui/bool_invalid.h"
#include "../BANK_spui/font_invalid.h"
#include "../BANK_spui/font_not_found.h"
#include "../BANK_spui/not_zero.h"
#include "../BANK_spui/unknown_setting.h"
#include "../common/memory.h"
#include "../liblayer3/liblayer3.h"
#include "ini_common.h"
#include "show_errors.h"
#include "string_is_false.h"
#include "string_is_true.h"
#include "trim_whitespace.h"
#include <stdio.h>
#include <string.h>

uint8_t SettingsTempValue;
void settings_apply(const char *Command) {
	IniKey = trim_whitespace(strtok(Command, "="));
	IniValue = trim_whitespace(strtok(NULL, "\n"));

	if(!stricmp(IniKey, "errors")) {
		// Error handling for settings/colon commands
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
			_farWithPointer(BANK_SPUI, spui_bool_invalid, IniKey);
		}
		return;
	} else

	if(!stricmp(IniKey, "font")) {
		// Use bundled font
		if(IniValue[0] != '/') {
			SettingsTempValue = _farWithPointer(BANK_FONTS, font_get, IniValue);
			// -1 means we did not find a font
			if(SettingsTempValue != -1) {
				_farWithUChar(BANK_FONTS, font_set, SettingsTempValue);
				return;
			}
			else {
				// ERROR
				_farWithPointer(BANK_SPUI, spui_font_invalid, IniValue);
				return;
			}

		}
		else if (SettingsShowErrors) {
			_farWithPointer(BANK_SPUI, spui_font_not_found, IniValue);
		}
		return;
	} else

	if(!stricmp(IniKey, "repeat_start")) {
		SettingsTempValue = atoi(IniValue);
		if(SettingsTempValue) {
			L3RepeatStart = SettingsTempValue;
		}
		else if (SettingsShowErrors) {
			_far(BANK_SPUI, spui_not_zero);
		}
		return;
	} else

	if(!stricmp(IniKey, "repeat_key")) {
		SettingsTempValue = atoi(IniValue);
		if(SettingsTempValue) {
			L3RepeatKey = SettingsTempValue;
		}
		else if (SettingsShowErrors) {
			_far(BANK_SPUI, spui_not_zero);
		}
		return;
	} else

	if(!stricmp(IniKey, "cursor_flash")) {
		SettingsTempValue = atoi(IniValue);
		if(SettingsTempValue) {
			L3CursorFlashRate = SettingsTempValue;
		}
		else if (SettingsShowErrors) {
			_far(BANK_SPUI, spui_not_zero);
		}
		return;
	} else
	if (SettingsShowErrors) {
		_farWithPointer(BANK_SPUI, spui_unknown_setting, IniKey);
	}
	return;
}