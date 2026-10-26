//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arch/zxn.h>

#include "../BANK_settings/apply.h"
#include "../common/evil.h"
#include "../common/file.h"
#include "../common/memory.h"
#include "../BANK_system/system.h"
#include "error.h"
#include "file_name_set.h"

void command_parse(void) {
	// Get the first part of the command (we space delimit parameters)
	char* w = strtok(Buffer, " ");
	if (!*w) {
		return;
	}

	char* arg = strtok(NULL, "");
	switch (*w)
	{
		case 's':		// S is for SETTINGS, or SET ;-)
		{
			if(!stricmp(w, "set")) {
				strcat(arg, "\n");
				_farWithPointer(BANK_SETTINGS, (FarFnPointer)settings_apply, arg);
			}
			break;
		}

		case 'w':
		{
			bool quitting = (w[1] == 'q');
			if (arg) {
				command_file_name_set(arg);
			}
			else {
				CommandPending = quitting ? COMMAND_SAVE_QUIT : COMMAND_SAVE;
			}
			break;
		}

		case 'r':
		{
			if (arg)
			{
				CommandPending = COMMAND_READ;
				CommandArg = arg;
			}
			else
				command_error_no_filename();
			break;
		}
		case 'e':
		{
			if (!arg)
				command_error_no_filename();
			else if (EvilDirtyFlag && (w[1] != '!'))
				command_error_document_not_saved();
			else
			{
				command_file_name_set(arg);
				CommandPending = COMMAND_LOAD;
			}
			break;
		}
		case 'n':
		{
			if (EvilDirtyFlag && (w[1] != '!')) {
				command_error_document_not_saved();
			}
			else {
				file_new();
				FileName = 0;
			}
			break;
		}
		case 'q':
		{
			if (!EvilDirtyFlag || (w[1] == '!'))
				quit();
			else
				command_error_document_not_saved();
			break;
		}

		default:
			_farWithPointer(BANK_COMMAND, (FarFnPointer)print_status, "Unknown Command");
	}
}