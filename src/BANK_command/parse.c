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
#include "file_save.h"

void command_parse() {
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
				_farWithPointer(BANK_SETTINGS, (void *(*)(void *))settings_apply, arg);
			}
			break;
		}

		case 'w':
		{
			bool quitting = (w[1] == 'q');
			if (arg) {
				command_file_name_set(arg);
			}
			else if (command_file_save())
			{
				if (quitting)
					quit();
			}
			break;
		}

		case 'r':
		{
			if (arg)
			{
				char *backupfcb = malloc(sizeof(FileName));
				memcpy(&backupfcb, &FileName, sizeof(backupfcb));
				FileName = arg;
				insert_file();
				memcpy(&FileName, &backupfcb, sizeof(backupfcb));
				free(backupfcb);
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
				load_file();
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
			_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, "Unknown Command");
	}
}