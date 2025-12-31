//
// Created by D Rimron-Soutter on 12/11/2020.
//

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
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include "file.h"
#include "evil.h"

#include "../BANK_system/system.h"
#include "../common/file.h"
#include "../common/memory.h"
char* FileName = 0;

const char *tmp_filename = "EVILTEMP.$$$";

void file_new(void)
{
	GapStart = BufferStart;
	GapEnd = BufferEnd;

	FirstLine = CurrentLine = BufferStart;
	EvilDirtyFlag = true;
}

bool really_save_file(const char* fcb)
{
	const uint8_t* inp;
	uint8_t* outp;
	static uint16_t pushed;

	strcpy(MessageBuffer, "Writing ");
	strcat(MessageBuffer, fcb);
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);

	errno = 0;
	FileHandle = esxdos_f_open(fcb, ESXDOS_MODE_W | ESXDOS_MODE_CT);
	if (errno) {
		goto error;

	}

	inp = BufferStart;
	outp = cpm_default_dma;
	pushed = 0;
	while ((inp != BufferEnd) || (outp != cpm_default_dma) || pushed)
	{
		static uint16_t c;

		if (pushed)
		{
			c = pushed;
			pushed = 0;
		}
		else
		{
			if (inp == GapStart)
				inp = GapEnd;
			c = (inp != BufferEnd) ? *inp++ : 0;

//			if (c == '\n')
//			{
//				pushed = '\n';
//				c = '\r';
//			}
		}

		if(c) *outp++ = c;

		zx_border(0);
		if (outp == (cpm_default_dma+128))
		{
			esx_f_write(FileHandle, cpm_default_dma, 128);
			if(errno)
				goto error;
			outp = cpm_default_dma;
		}

		zx_border(2);
		// special case to get around CPMs 128b block
		if ((inp == BufferEnd) && !pushed && (outp != cpm_default_dma)) {
			uint8_t b = outp - cpm_default_dma;

			esx_f_write(FileHandle, cpm_default_dma, outp - cpm_default_dma);
			if(errno)
				goto error;
			outp = cpm_default_dma;
		}
	}

	EvilDirtyFlag = false;
	esxdos_f_close(FileHandle);
	return true;

	error:
	strcpy(MessageBuffer, "Failed to save file (errno:");
	itoa(errno, MessageBuffer + strlen(MessageBuffer), 10);
	strcat(MessageBuffer, ")");
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);
	_far(BANK_SYSTEM,system_beep);
	return false;

	esxdos_f_close(FileHandle);

	return false;
}
