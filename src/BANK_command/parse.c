//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arch/zxn.h>
#include <arch/zx/esxdos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

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
	if (!w) {
		return;
	}

	char* arg = strtok(NULL, "");
	switch (*w)
	{
		case 's':		// S is for SETTINGS, or SET ;-)
		{
			if(arg && !stricmp(w, "set")) {
				strcat(arg, "\n");
				_farWithPointer(BANK_SETTINGS, (FarFnPointer)settings_apply, arg);
			}
			break;
		}

		case 'w':
		{
			bool quitting = (w[1] == 'q');
			// :w NAME does not write over another file that exists, unless :w! NAME
			if (arg && w[quitting ? 2 : 1] != '!' && (!FileName || stricmp(arg, FileName))) {
				errno = 0;
				unsigned char handle = esxdos_f_open(arg, ESXDOS_MODE_R);
				if (!errno) {
					esxdos_f_close(handle);
					command_error_file_exists();
					break;
				}
			}
			CommandPending = quitting ? COMMAND_SAVE_QUIT : COMMAND_SAVE;
			CommandArg = arg;
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
				// colon() loads the file, and sets the name if it fits
				CommandPending = COMMAND_LOAD;
				CommandArg = arg;
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