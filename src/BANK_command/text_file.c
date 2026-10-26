//
// Reading and writing the text, straight between files and the text's pages.
//
// The block code is in common/text.c. This is in BANK_COMMAND, not main: main is full. It can
// use the text because BANK_COMMAND fits in its first 8K page, at 0xC000, and the text goes in
// at 0xE000.
//

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../common/memory.h"
#include "../common/text.h"

// Reads the rest of an open file in at Cursor, straight into the blocks, and moves Cursor after
// it. CRs are dropped. False when the memory is full, or on a read error (errno is set).
bool text_read(uint8_t handle)
{
	static uint8_t b;
	static uint16_t used;
	uint8_t i;
	bool full = false;
	bool at_end = text_is_end(&Cursor);

	// The text goes on the end of block b. In the middle of the text, the text after the cursor
	// moves to a block of its own first, so that it does not move again for every read.
	if (at_end)
		b = Cursor.blk;
	else
	{
		if (TextBlocks >= TextPages - TEXT_RESERVE)
			return false;
		if (Cursor.off)
		{
			text_block_add(Cursor.blk + 1);
			text_block_move(Cursor.blk, TextUsed[Cursor.blk] - Cursor.off);
		}
		else
			text_block_add(Cursor.blk);
		b = Cursor.blk - 1;
		used = TextUsed[b];
		// The other marks at the cursor stay before the new text
		for (i = 1; i < TEXT_MARKS; i++)
			if (text_eq(&TextMark[i], &Cursor))
			{
				TextMark[i].blk = b;
				TextMark[i].off = used;
			}
	}

	for (;;)
	{
		uint8_t *start;
		uint8_t *cr;
		uint16_t got;

		used = TextUsed[b];
		if (used == TEXT_BLOCK_SIZE)
		{
			if (TextBlocks >= TextPages - TEXT_RESERVE)
			{
				// Full, unless the file has ended too
				full = esx_f_read(handle, Buff512, 1) != 0;
				break;
			}
			text_block_add(++b);
			used = 0;
		}

		text_map(b);
		start = TEXT_WINDOW + used;
		got = esx_f_read(handle, start, TEXT_BLOCK_SIZE - used);
		if (errno || !got)
			break;

		cr = memchr(start, '\r', got);
		if (cr)
		{
			uint8_t *out = cr;
			uint8_t *end = start + got;
			while (cr != end)
			{
				uint8_t c = *cr++;
				if (c != '\r')
					*out++ = c;
			}
			got = out - start;
		}
		TextUsed[b] = used + got;
	}

	if (at_end)
		text_end(&Cursor);
	// An empty block left at the end of the reading goes
	if (!TextUsed[b])
		text_tidy(b);
	for (i = 0; i < TEXT_MARKS; i++)
		text_fix(&TextMark[i]);
	return !full && !errno;
}

// Writes the whole text to an open file, straight from the blocks
bool text_write(uint8_t handle)
{
	uint8_t b;

	for (b = 0; b < TextBlocks; b++)
	{
		if (!TextUsed[b])
			continue;
		text_map(b);
		esx_f_write(handle, TEXT_WINDOW, TextUsed[b]);
		if (errno)
			return false;
	}
	return true;
}
