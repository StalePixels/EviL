//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "file_save.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#include <z80.h>
#include <intrinsic.h>
#include <arch/zxn.h>
#include <arch/zx/esxdos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include "error.h"

#include "../common/file.h"
#include "../common/evil.h"
#include "../common/memory.h"

bool command_file_save(void)
{
	if (!FileName) {
		command_error_no_filename();
		return false;
	}


	errno = 0;
	FileHandle = esxdos_f_open(FileName, ESX_MODE_OPEN_CREAT_NOEXIST);
	if(errno == ESX_EEXIST)
		goto file_exists;

	esxdos_f_close(FileHandle);
	if (!errno) {
		/* The file does not exist. */
		if (really_save_file(FileName)) {
			EvilDirtyFlag = false;
			return true;
		}
	}

	strcpy(message_buffer, "Failed to save file (errno:");
	itoa(errno, message_buffer+strlen(message_buffer), 10);
	strcat(message_buffer, ")");
	_farWithPointer(BANK_COMMAND, (void (*)(void *)) print_status, message_buffer);

	_far(BANK_SYSTEM,system_beep);

	return false;

	file_exists:
	/* Write to a temporary file. */

	esxdos_f_close(FileHandle);
	if (really_save_file(FileNameTemp) == false)
		goto tempfile;

	strcpy(message_buffer, "Renaming ");
	strcat(message_buffer, FileNameTemp);
	strcat(message_buffer, " to ");
	strcat(message_buffer, FileName);
	_farWithPointer(BANK_COMMAND, (void (*)(void *)) print_status, message_buffer);

	errno = 0;
	esxdos_f_unlink(FileName);
	if (errno)
		goto commit;
	esx_f_rename(FileNameTemp, FileName);
	if (errno)
		goto commit;

	return true;

	tempfile:
	strcpy(message_buffer, "Cannot create EVILTEMP.$$$ file - it may exist (errno:");
	itoa(errno, message_buffer+strlen(message_buffer), 10);
	strcat(message_buffer, ")");
	_farWithPointer(BANK_COMMAND, (void (*)(void *)) print_status, message_buffer);
	_far(BANK_SYSTEM,system_beep);
	return false;

	commit:
	strcpy(message_buffer, "Cannot commit file; your data may be in EVILTEMP.$$$ (errno:");
	itoa(errno, message_buffer+strlen(message_buffer), 10);
	strcat(message_buffer, ")");
	_farWithPointer(BANK_COMMAND, (void (*)(void *)) print_status, message_buffer);
	_far(BANK_SYSTEM,system_beep);
	return false;
}