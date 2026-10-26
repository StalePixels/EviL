//
// Loads a font from a .ch8 file
//
// The file holds characters 32 to 127, 8 bytes each, as the bundled fonts do after decompression.
// A file of any other size is refused. On a refusal or a read error the font in use stays.
//

#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <string.h>

#include "load.h"
#include "set.h"

uint8_t font_load(const char *file_name) {
	struct esx_stat stat;
	uint8_t handle;
	uint8_t result = FONT_LOAD_NOT_READ;

	errno = 0;
	handle = esxdos_f_open(file_name, ESXDOS_MODE_R);
	if (errno)
		return FONT_LOAD_NOT_READ;

	esx_f_fstat(handle, &stat);
	if (errno)
		goto done;
	// Two 16-bit compares: with one 32-bit compare, zsdcc does not test the low byte, and takes 769 bytes
	if (((uint16_t)stat.size != FONT_SIZE) || (uint16_t)(stat.size >> 16)) {
		result = FONT_LOAD_WRONG_SIZE;
		goto done;
	}

	// The file goes straight to the tile definitions: keep the font in use, for a read error
	memcpy(FontBackup, FONT_DEFINITIONS, FONT_SIZE);
	if ((esxdos_f_read(handle, FONT_DEFINITIONS, FONT_SIZE) != FONT_SIZE) || errno) {
		memcpy(FONT_DEFINITIONS, FontBackup, FONT_SIZE);
		goto done;
	}
	FontCurrent = FONT_NONE;
	result = FONT_LOAD_OK;

done:
	esxdos_f_close(handle);
	return result;
}
