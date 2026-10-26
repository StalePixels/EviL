//
// Reading and saving files.
//
// BANK_COMMAND fits in its first 8K page, at 0xC000, so this code can use the text: the text
// goes in at 0xE000 (common/text.c).
//

#include <arch/zxn.h>
#include <arch/zx/esxdos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "load_save.h"
#include "error.h"
#include "../BANK_system/beep.h"
#include "../common/evil.h"
#include "../common/file.h"
#include "../common/memory.h"
#include "../common/text.h"

static void file_error(const char* message)
{
	strcpy(MessageBuffer, message);
	strcat(MessageBuffer, FileName);
	strcat(MessageBuffer, " (errno:");
	itoa(errno, MessageBuffer +strlen(MessageBuffer), 10);
	strcat(MessageBuffer, ")");
	print_status(MessageBuffer);
}

static void save_error(const char* message)
{
	strcpy(MessageBuffer, message);
	itoa(errno, MessageBuffer +strlen(MessageBuffer), 10);
	strcat(MessageBuffer, ")");
	print_status(MessageBuffer);
	_far(BANK_SYSTEM, system_beep);
}

// The bytes a file read is sure to get: the room in the block it fills first, and the free pages
// after the reserve. In the middle of the text, one page takes the text after the cursor first.
static uint32_t file_room(uint8_t replace)
{
	uint8_t spare;
	uint16_t first;

	if (replace)
		return (uint32_t)(TextPages - TEXT_RESERVE) * TEXT_BLOCK_SIZE;

	spare = TextPages - TextBlocks;
	if (text_is_end(&Cursor))
		first = TEXT_BLOCK_SIZE - TextUsed[Cursor.blk];
	else
	{
		if (spare <= TEXT_RESERVE)
			return 0;
		spare--;
		first = TEXT_BLOCK_SIZE - Cursor.off;
	}
	if (spare <= TEXT_RESERVE)
		return first;
	return first + (uint32_t)(spare - TEXT_RESERVE) * TEXT_BLOCK_SIZE;
}

// Reads FileName in at the cursor. With replace, the file replaces the text.
// A file bigger than the free space is refused and the text stays as it was: then 0.
void *command_file_insert(uint8_t replace)
{
	struct esx_stat stat;
	uint32_t room;

	strcpy(MessageBuffer, "Reading ");
	strcat(MessageBuffer, FileName);
	print_status(MessageBuffer);

	errno = 0;
	FileHandle = esxdos_f_open(FileName, ESXDOS_MODE_R);
	if (errno)
	{
		if (replace)
			file_new();
		file_error("Could not read file ");
		return (void *)1;
	}

	room = file_room(replace);
	esx_f_fstat(FileHandle, &stat);
	if (errno)
		goto error;
	if (stat.size > room)
	{
		esxdos_f_close(FileHandle);
		strcpy(MessageBuffer, "File too big: ");
		ultoa(stat.size, MessageBuffer +strlen(MessageBuffer), 10);
		strcat(MessageBuffer, " bytes, ");
		ultoa(room, MessageBuffer +strlen(MessageBuffer), 10);
		strcat(MessageBuffer, " free");
		print_status(MessageBuffer);
		_far(BANK_SYSTEM, system_beep);
		return 0;
	}

	if (replace)
		file_new();

	if (!text_read(FileHandle))
	{
		if (errno)
			goto error;
		print_status("Out of memory");
	}
	goto done;

error:
	file_error("Could not read file ");
done:
	esxdos_f_close(FileHandle);
	EvilDirtyFlag = true;
	return (void *)1;
}

static bool file_write(const char* fcb)
{
	strcpy(MessageBuffer, "Writing ");
	strcat(MessageBuffer, fcb);
	print_status(MessageBuffer);

	errno = 0;
	FileHandle = esxdos_f_open(fcb, ESXDOS_MODE_W | ESXDOS_MODE_CT);
	if (errno)
		goto error;

	if (!text_write(FileHandle))
	{
		esxdos_f_close(FileHandle);
		goto error;
	}

	EvilDirtyFlag = false;
	esxdos_f_close(FileHandle);
	return true;

error:
	save_error("Failed to save file (errno:");
	return false;
}

// An existing file is written as EVILTEMP.$$$ first, then renamed. 0 when the save failed.
void *command_file_save(void)
{
	static const char tempfcb[] = "EVILTEMP.$$$";

	if (!FileName) {
		command_error_no_filename();
		return 0;
	}

	errno = 0;
	FileHandle = esxdos_f_open(FileName, ESX_MODE_OPEN_CREAT_NOEXIST);
	if (errno == ESX_EEXIST) {
		goto file_exists;
	}

	esxdos_f_close(FileHandle);
	if (!errno) {
		/* The file does not exist. */
		if (file_write(FileName))
			return (void *)1;
	}

	save_error("Failed to save file (errno:");
	return 0;

file_exists:
	/* Write to a temporary file. */

	esxdos_f_close(FileHandle);
	if (file_write(tempfcb) == false)
		goto tempfile;

	strcpy(MessageBuffer, "Renaming ");
	strcat(MessageBuffer, tempfcb);
	strcat(MessageBuffer, " to ");
	strcat(MessageBuffer, FileName);
	print_status(MessageBuffer);

	errno = 0;
	esxdos_f_unlink(FileName);
	if (errno)
		goto commit;
	esx_f_rename(tempfcb, FileName);
	if (errno)
		goto commit;

	return (void *)1;

tempfile:
	save_error("Cannot create EVILTEMP.$$$ file - it may exist (errno:");
	return 0;

commit:
	save_error("Cannot commit file; your data may be in EVILTEMP.$$$ (errno:");
	return 0;
}
