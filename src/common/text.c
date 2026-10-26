//
// The text, kept in 8K RAM pages.
//
// At startup EviL takes every 8K page NextZXOS gives it (IDE_BANK), plus the two pages the dotn
// loader gave main at 0xC000. Each page holds one block. TextPage[] and TextUsed[] list the
// blocks in text order: TextPage[b] is the page of block b, TextUsed[b] how many bytes of it
// hold text. The text of a block starts at the start of its page and its free space follows.
// The entries from TextBlocks to TextPages-1 are the pages that hold no text. The two lists are
// in the dot command's own 8K at 0x2000 (text_blocks.asm).
//
// A block is read and written at 0xE000, through MMU7. Text moves from one block to another
// through Buff512, so only MMU7 changes. Code that calls these functions must not be in
// 0xE000-0xFFFF: it is in main, or in BANK_COMMAND, which fits in its first 8K page.
// Files are read into and written from the pages directly, in BANK_command/text_file.c.
//
// The rules:
// - There is always at least one block. Only the text of an empty file is an empty block.
// - A place is a block and an offset, and each place in the text has one form: the offset is
//   less than the block's used bytes, except at the end of the text, which is the used bytes
//   of the last block.
// - A full block splits in two when text goes into it. A block with less than TEXT_SPARSE
//   bytes joins a neighbour when text leaves it, if the two fit in one block. An empty block
//   goes back to the free pages.
// - Cursor, CurrentLine and FirstLine (TextMark) stay on the same text when blocks split,
//   join or move. Text goes in and out only at Cursor, and the other marks are never after it.
//

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "evil.h"
#include "memory.h"
#include "text.h"

struct TextPos TextMark[3];

uint8_t TextBlocks;
uint8_t TextPages;

void text_map(uint8_t blk)
{
	ZXN_WRITE_MMU7(TextPage[blk]);
}

// Main's own two pages at 0xC000 are the first; the loader frees those
void text_init(void)
{
	TextPage[0] = OriginalMMU6;
	TextPage[1] = OriginalMMU7;
	TextPages = 2;

	while (TextPages < TEXT_MAX_BLOCKS)
	{
		uint8_t page = esx_ide_bank_alloc(ESX_BANKTYPE_RAM);
		if (page == 0xFF)
			break;
		TextPage[TextPages++] = page;
	}

	text_clear();
}

void text_release(void)
{
	uint8_t i;

	for (i = 2; i < TextPages; i++)
		esx_ide_bank_free(ESX_BANKTYPE_RAM, TextPage[i]);
	TextPages = 2;
}

// The pages stay where they are in the list: which page holds which block does not matter
void text_clear(void)
{
	TextBlocks = 1;
	TextUsed[0] = 0;
	memset(TextMark, 0, sizeof(TextMark));
}

uint32_t text_capacity(void)
{
	return (uint32_t)TextPages * TEXT_BLOCK_SIZE;
}

uint32_t text_free(void)
{
	uint32_t free = text_capacity();
	uint8_t i;

	for (i = 0; i < TextBlocks; i++)
		free -= TextUsed[i];
	return free;
}

/* ======================================================================= */
/*                                 PLACES                                  */
/* ======================================================================= */

// The end of the text reads as a newline
uint8_t text_get(const struct TextPos *p)
{
	if (p->off == TextUsed[p->blk])
		return '\n';
	text_map(p->blk);
	return TEXT_WINDOW[p->off];
}

void text_put(const struct TextPos *p, uint8_t c)
{
	if (p->off == TextUsed[p->blk])
		return;
	text_map(p->blk);
	TEXT_WINDOW[p->off] = c;
}

bool text_is_start(const struct TextPos *p)
{
	return !p->blk && !p->off;
}

bool text_is_end(const struct TextPos *p)
{
	return p->off == TextUsed[p->blk];
}

bool text_eq(const struct TextPos *a, const struct TextPos *b)
{
	return (a->blk == b->blk) && (a->off == b->off);
}

bool text_lt(const struct TextPos *a, const struct TextPos *b)
{
	if (a->blk != b->blk)
		return a->blk < b->blk;
	return a->off < b->off;
}

// An offset at the end of a block that is not the last is the start of the next block
void text_fix(struct TextPos *p)
{
	if ((p->off == TextUsed[p->blk]) && (p->blk + 1 < TextBlocks))
	{
		p->blk++;
		p->off = 0;
	}
}

void text_next(struct TextPos *p)
{
	if (p->off == TextUsed[p->blk])
		return;
	p->off++;
	text_fix(p);
}

void text_prev(struct TextPos *p)
{
	if (p->off)
		p->off--;
	else if (p->blk)
	{
		p->blk--;
		p->off = TextUsed[p->blk] - 1;
	}
}

// Maps p's block in and returns p's address in it; n gets the bytes from p to the end of the block.
// The address is good until something else maps a block.
uint8_t *text_span(const struct TextPos *p, uint16_t *n)
{
	text_map(p->blk);
	*n = TextUsed[p->blk] - p->off;
	return TEXT_WINDOW + p->off;
}

// The character before p; the start of the text reads as a newline
uint8_t text_before(const struct TextPos *p)
{
	struct TextPos q = *p;

	if (text_is_start(&q))
		return '\n';
	text_prev(&q);
	return text_get(&q);
}

// Stops at the end of the text
void text_advance(struct TextPos *p, uint16_t n)
{
	while (n)
	{
		uint16_t left = TextUsed[p->blk] - p->off;
		if (n < left)
		{
			p->off += n;
			return;
		}
		n -= left;
		p->off += left;
		if (p->blk + 1 == TextBlocks)
			return;
		text_fix(p);
	}
}

// from must not be after to
uint32_t text_distance(const struct TextPos *from, const struct TextPos *to)
{
	uint32_t n;
	uint8_t b;

	if (from->blk == to->blk)
		return to->off - from->off;

	n = TextUsed[from->blk] - from->off;
	for (b = from->blk + 1; b != to->blk; b++)
		n += TextUsed[b];
	return n + to->off;
}

void text_end(struct TextPos *p)
{
	p->blk = TextBlocks - 1;
	p->off = TextUsed[p->blk];
}

// To the newline that ends p's line, or the end of the text
void text_line_end(struct TextPos *p)
{
	for (;;)
	{
		uint16_t used = TextUsed[p->blk];
		uint8_t *nl;

		text_map(p->blk);
		nl = memchr(TEXT_WINDOW + p->off, '\n', used - p->off);
		if (nl)
		{
			p->off = nl - TEXT_WINDOW;
			return;
		}
		p->off = used;
		if (p->blk + 1 == TextBlocks)
			return;
		text_fix(p);
	}
}

// To the start of p's line: the place after the last newline before p, or the start of the text
void text_line_start(struct TextPos *p)
{
	for (;;)
	{
		uint8_t *c;

		text_map(p->blk);
		c = TEXT_WINDOW + p->off;
		while (c != TEXT_WINDOW)
		{
			if (*--c == '\n')
			{
				p->off = c - TEXT_WINDOW + 1;
				text_fix(p);
				return;
			}
		}
		p->off = 0;
		if (!p->blk)
			return;
		p->blk--;
		p->off = TextUsed[p->blk];
	}
}

/* ======================================================================= */
/*                                 BLOCKS                                  */
/* ======================================================================= */

// A free page becomes an empty block, block at
void text_block_add(uint8_t at)
{
	uint8_t page = TextPage[TextBlocks];
	uint8_t i;

	memmove(&TextPage[at + 1], &TextPage[at], TextBlocks - at);
	memmove(&TextUsed[at + 1], &TextUsed[at], (TextBlocks - at) * sizeof(TextUsed[0]));
	TextPage[at] = page;
	TextUsed[at] = 0;
	TextBlocks++;

	for (i = 0; i < TEXT_MARKS; i++)
		if (TextMark[i].blk >= at)
			TextMark[i].blk++;
}

// Empty block b goes back to the free pages. No mark is in it.
static void text_block_remove(uint8_t b)
{
	uint8_t page = TextPage[b];
	uint8_t i;

	TextBlocks--;
	memmove(&TextPage[b], &TextPage[b + 1], TextBlocks - b);
	memmove(&TextUsed[b], &TextUsed[b + 1], (TextBlocks - b) * sizeof(TextUsed[0]));
	TextPage[TextBlocks] = page;

	for (i = 0; i < TEXT_MARKS; i++)
		if (TextMark[i].blk > b)
			TextMark[i].blk--;
}

// The last n bytes of block b go to the front of block b+1, which has room for them
void text_block_move(uint8_t b, uint16_t n)
{
	uint16_t from = TextUsed[b] - n;
	uint16_t done = 0;
	uint8_t i;

	text_map(b + 1);
	memmove(TEXT_WINDOW + n, TEXT_WINDOW, TextUsed[b + 1]);

	while (done != n)
	{
		uint16_t chunk = n - done;
		if (chunk > sizeof(Buff512))
			chunk = sizeof(Buff512);

		text_map(b);
		memcpy(Buff512, TEXT_WINDOW + from + done, chunk);
		text_map(b + 1);
		memcpy(TEXT_WINDOW + done, Buff512, chunk);
		done += chunk;
	}

	TextUsed[b] = from;
	TextUsed[b + 1] += n;

	for (i = 0; i < TEXT_MARKS; i++)
	{
		struct TextPos *m = &TextMark[i];
		if (m->blk == b + 1)
			m->off += n;
		else if ((m->blk == b) && (m->off >= from))
		{
			m->blk++;
			m->off -= from;
		}
	}
}

// Cursor's block is full. Splitting at Cursor puts the text after it in a new block and leaves
// the cursor at the start of that block's free space; splitting in the middle leaves room on
// both sides. The larger of the two places is used, so the first block keeps at least half.
static bool text_split(void)
{
	uint8_t b = Cursor.blk;
	uint16_t at = TextUsed[b] / 2;

	if (TextBlocks == TextPages)
		return false;

	if (Cursor.off > at)
		at = Cursor.off;
	text_block_add(b + 1);
	text_block_move(b, TextUsed[b] - at);
	return true;
}

// After text leaves block b: an empty block goes, a sparse one joins a neighbour
void text_tidy(uint8_t b)
{
	uint8_t i;

	if (!TextUsed[b])
	{
		if (TextBlocks == 1)
			return;
		// A mark in the empty block is at its offset 0: the text after it starts the next
		// block, or, if it was the last, the mark is at the end of the block before
		for (i = 0; i < TEXT_MARKS; i++)
		{
			struct TextPos *m = &TextMark[i];
			if (m->blk == b)
			{
				if (b + 1 == TextBlocks)
				{
					m->blk = b - 1;
					m->off = TextUsed[b - 1];
				}
				else
					m->blk++;
			}
		}
		text_block_remove(b);
	}
	else if (TextUsed[b] < TEXT_SPARSE)
	{
		if ((b + 1 < TextBlocks) && (TextUsed[b] + TextUsed[b + 1] <= TEXT_BLOCK_SIZE))
		{
			text_block_move(b, TextUsed[b]);
			text_block_remove(b);
		}
		else if (b && (TextUsed[b - 1] + TextUsed[b] <= TEXT_BLOCK_SIZE))
		{
			text_block_move(b - 1, TextUsed[b - 1]);
			text_block_remove(b - 1);
		}
	}

	for (i = 0; i < TEXT_MARKS; i++)
		text_fix(&TextMark[i]);
}

/* ======================================================================= */
/*                                 CHANGES                                 */
/* ======================================================================= */

// Puts n bytes from main memory in at Cursor, and moves Cursor after them.
// False when the memory is full; the bytes that fitted stay in.
bool text_insert(const uint8_t *src, uint16_t n)
{
	while (n)
	{
		uint8_t b = Cursor.blk;
		uint16_t off = Cursor.off;
		uint16_t used = TextUsed[b];
		uint16_t room = TEXT_BLOCK_SIZE - used;

		if (!room)
		{
			if (!text_split())
				return false;
			continue;
		}
		if (room > n)
			room = n;

		text_map(b);
		memmove(TEXT_WINDOW + off + room, TEXT_WINDOW + off, used - off);
		memcpy(TEXT_WINDOW + off, src, room);
		TextUsed[b] = used + room;
		Cursor.off = off + room;
		text_fix(&Cursor);
		src += room;
		n -= room;
	}
	return true;
}

bool text_insert_char(uint8_t c)
{
	return text_insert(&c, 1);
}

// Takes out up to n bytes at Cursor, fewer at the end of the text
void text_delete(uint32_t n)
{
	while (n)
	{
		uint8_t b = Cursor.blk;
		uint16_t off = Cursor.off;
		uint16_t used = TextUsed[b];
		uint16_t gone = used - off;

		if (!gone)
			return;
		if (gone > n)
			gone = n;

		text_map(b);
		memmove(TEXT_WINDOW + off, TEXT_WINDOW + off + gone, used - off - gone);
		TextUsed[b] = used - gone;
		n -= gone;
		text_tidy(b);
	}
}
