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
#include <arch/zx/esxdos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include "file.h"
#include "evil.h"

#include "../common/memory.h"
#include "../common/file.h"
const char* FileName = 0;


void file_new(void)
{
	gap_start = buffer_start;
	gap_end = buffer_end;

	first_line = current_line = buffer_start;
	EvilDirtyFlag = true;
}

bool really_save_file(const char* fcb)
{
	const uint8_t* inp;
	uint8_t* outp;
	static uint16_t pushed;

	strcpy(message_buffer, "Writing ");
	strcat(message_buffer, fcb);
	print_status(message_buffer);

	errno = 0;
	FileHandle = esxdos_f_open(fcb, ESXDOS_MODE_W | ESXDOS_MODE_CT);
	if (errno) {
		strcpy(message_buffer, "Failed to create file (errno:");
		itoa(errno, message_buffer + strlen(message_buffer), 10);
		strcat(message_buffer, ")");
		print_status(message_buffer);
		_far(BANK_SYSTEM,system_beep);
		return false;
	}

	inp = buffer_start;
	outp = cpm_default_dma;
	pushed = 0;
	while ((inp != buffer_end) || (outp != cpm_default_dma) || pushed)
	{
		static uint16_t c;

		if (pushed)
		{
			c = pushed;
			pushed = 0;
		}
		else
		{
			if (inp == gap_start)
				inp = gap_end;
			c = (inp != buffer_end) ? *inp++ : 0;

//			if (c == '\n')
//			{
//				pushed = '\n';
//				c = '\r';
//			}
		}

		if(c) *outp++ = c;

		if (outp == (cpm_default_dma+128))
		{
			esx_f_write(FileHandle, cpm_default_dma, 128);
			if(errno)
				goto error;
			outp = cpm_default_dma;
		}

		// special case to get around CPMs 128b block
		if ((inp == buffer_end) && !pushed && (outp != cpm_default_dma)) {
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
	esxdos_f_close(FileHandle);
	return false;
}
