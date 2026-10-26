/*
 * EVIL, for Spectrum Next, by D. Rimron-Soutter
 *
 * BEGAN AS:   QE © 2019 David Given
 * This library is distributable under the terms of the .
 * See COPYING.cpmish in the distribution root directory for more information.
 *
 */

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

#include "common/build.h"
#include "common/evil.h"
#include "common/memory.h"
#include "common/file.h"
#include "common/help.h"
#include "common/text.h"
#include "liblayer3/liblayer3.h"

#include "BANK_command/parse.h"
#include "BANK_command/status.h"
#include "BANK_command/error.h"
#include "BANK_command/file_name_set.h"
#include "BANK_command/load_save.h"
#include "BANK_system/system.h"

uint8_t L3ScreenX = 0, L3ScreenY = 0;
uint8_t OriginalMMU6, OriginalMMU7, FileHandle;

void (*print_status)(const char*);

bool EvilDirtyFlag;

uint16_t CurrentLineY;
uint8_t DisplayHeight[HEIGHT];
uint16_t LineLength[HEIGHT];

uint16_t CommandCount;
uint16_t OperatorCount = 1;		// count typed before d, c or Z
typedef void command_t(uint16_t);

struct bindings
{
	const char* name;
	const char* keys;
	command_t* const* callbacks;
	uint8_t help;
};

const struct bindings* bindings;

extern const struct bindings DeleteBindings;
extern const struct bindings ZedBindings;
extern const struct bindings ChangeBindings;

char Buffer[128];
char MessageBuffer[128];

uint8_t CommandPending;
char* CommandArg;

extern void colon(uint16_t count);
extern void goto_line(uint16_t lineno);

/* ======================================================================= */
/*                              BUFFER MANAGEMENT                          */
/* ======================================================================= */

// The width on screen of the text from from to to, or to the end of from's line. from <= to.
uint16_t compute_length(const struct TextPos* from, const struct TextPos* to)
{
	static struct TextPos p;
	static uint16_t xo;
	static uint16_t n;
	static uint8_t* s;
	uint8_t c;

	p = *from;
	xo = 0;
	n = 0;
	for (;;)
	{
		// One block at a time: p moves to the end of the part s reads
		if (!n)
		{
			if (text_eq(&p, to) || text_is_end(&p))
				break;
			s = text_span(&p, &n);
			if (p.blk == to->blk)
				n = to->off - p.off;
			text_advance(&p, n);
		}

		c = *s++;
		n--;
		if (c == '\n')
			break;
		if (c == '\t')
			xo = (xo + 8) & ~7;
		else if (c < 32)
			xo += 2;
		else
			xo++;
	}

	return xo;
}

// Draws the line at p, and moves p to the start of the next line
void draw_line(struct TextPos* p)
{
	static struct TextPos q;
	static uint16_t n;
	static uint8_t* s;
	uint16_t xo = 0;
	uint16_t length = 0;
	uint8_t c;
	uint16_t starty = L3ScreenY;

	q = *p;
	n = 0;
	while (L3ScreenY != HEIGHT)
	{
		// One block at a time: q moves to the end of the part s reads
		if (!n)
		{
			if (text_is_end(&q))
			{
				if (xo == 0 && L3ScreenY < HEIGHT-1)
					l3_puts("~");
				l3_clear_to_eol();
				l3_newline();
				break;
			}
			s = text_span(&q, &n);
			text_advance(&q, n);
		}

		c = *s++;
		n--;
		length++;
		if (c == '\n')
		{
			l3_clear_to_eol();
            l3_newline();
			break;
		}
		else if (c == '\t')
		{
			do
			{
				l3_putc(' ');
				xo++;
			}
			while (xo & 7);
		}
		else
		{
			l3_putc(c);
			xo++;
		}
	}

	DisplayHeight[starty] = (xo / WIDTH) + 1;
	LineLength[starty] = length;
	text_advance(p, length);
}

/* start <= Cursor */
void render_screen(const struct TextPos* start)
{
	static struct TextPos p;
	static bool found;
	unsigned i;
	for (i= L3ScreenY; i != HEIGHT; i++)
		DisplayHeight[i] = 0;

	// Past the end of the text every row starts at the end: the first of them is the line
	p = *start;
	found = false;
	while (L3ScreenY < HEIGHT)
	{
		if (!found && text_eq(&p, &CurrentLine))
		{
			CurrentLineY = L3ScreenY;
			found = true;
		}
		draw_line(&p);
	}
}

void adjust_scroll_position(void)
{
	static struct TextPos line_start;
	uint16_t total_height = 0;

	FirstLine = CurrentLine;
	while (!text_is_start(&FirstLine))
	{
		line_start = FirstLine;
		text_prev(&line_start);
		text_line_start(&line_start);

		total_height += (compute_length(&line_start, &FirstLine) / WIDTH) + 1;
		if (total_height > (HEIGHT/2))
			break;
		FirstLine = line_start;
	}

	l3_goto(0, 0);
	render_screen(&FirstLine);
}

void recompute_screen_position(void)
{
	static struct TextPos p;
	uint16_t length;

	if (text_lt(&CurrentLine, &FirstLine))
		adjust_scroll_position();

	for (;;)
	{
		p = FirstLine;
		CurrentLineY = 0;
		while (CurrentLineY < HEIGHT)
		{
			uint16_t height;

			if (text_eq(&p, &CurrentLine))
				break;

			height = DisplayHeight[CurrentLineY];
			text_advance(&p, LineLength[CurrentLineY]);

			CurrentLineY += height;
		}

		if ((CurrentLineY >= HEIGHT) ||
			((CurrentLineY + DisplayHeight[CurrentLineY]) > HEIGHT))
		{
			adjust_scroll_position();
		}
		else
			break;
	}

	length = compute_length(&CurrentLine, &Cursor);
	l3_goto(length % WIDTH, CurrentLineY + (length / WIDTH));

	tilemap[L3ScreenY][L3ScreenX].flags = tilemap[L3ScreenY][L3ScreenX].flags + 128;
}

void redraw_current_line(void)
{
	static struct TextPos nextp;
    uint8_t oldheight;

	oldheight = DisplayHeight[CurrentLineY];
	l3_goto(0, CurrentLineY);
	nextp = CurrentLine;
	draw_line(&nextp);
	if (oldheight != DisplayHeight[CurrentLineY])
		render_screen(&nextp);

	recompute_screen_position();
}

/* ======================================================================= */
/*                                LIFECYCLE                                */
/* ======================================================================= */

// False when the file is too big, and the text did not change
bool load_file(void)
{
	if (FileName)
	{
		if (!_farWithUChar(BANK_COMMAND, (FarFnUChar)command_file_insert, true))
			return false;
	}
	else
		file_new();

	EvilDirtyFlag = false;
	goto_line(1);
	return true;
}

bool save_file(void)
{
	return _far(BANK_COMMAND, (FarFn)command_file_save) != 0;
}

void quit(void)
{
    l3_clear();
	l3_puts("Goodbye!");
	exit(0);
}

/* ======================================================================= */
/*                            EDITOR OPERATIONS                            */
/* ======================================================================= */

void cursor_home(uint16_t count)
{
	Cursor = CurrentLine;
}

void cursor_end(uint16_t count)
{
	text_line_end(&Cursor);
}

void cursor_left(uint16_t count)
{
	while (count--)
	{
		if (!text_eq(&Cursor, &CurrentLine))
			text_prev(&Cursor);
	}
}

void cursor_right(uint16_t count)
{
	while (count--)
	{
		if (!text_is_end(&Cursor) && (text_get(&Cursor) != '\n'))
			text_next(&Cursor);
	}
}

// How far the cursor is into its line
static uint16_t cursor_column(void)
{
	uint32_t offset = text_distance(&CurrentLine, &Cursor);
	return (offset > UINT_MAX) ? UINT_MAX : offset;
}

void cursor_down(uint16_t count)
{
	while (count--)
	{
		uint16_t offset = cursor_column();
		cursor_end(1);
		if (text_is_end(&Cursor))
			return;

		text_next(&Cursor);
		CurrentLine = Cursor;
		cursor_right(offset);
	}
}

void cursor_up(uint16_t count)
{
	while (count--)
	{
		uint16_t offset = cursor_column();

		cursor_home(1);
		if (text_is_start(&Cursor))
			return;

		text_prev(&Cursor);
		text_line_start(&Cursor);

		CurrentLine = Cursor;
		cursor_right(offset);
	}
}

bool word_boundary(uint16_t left, uint16_t right)
{
	if (!isalnum(left) && isalnum(right))
		return 1;
	if (isspace(left) && !isspace(right))
		return 1;
	return 0;
}

void cursor_wordleft(uint16_t count)
{
	while (count--)
	{
		bool linechanged = false;

		while (!text_is_start(&Cursor))
		{
			uint16_t right, left;

			text_prev(&Cursor);
			right = text_get(&Cursor);
			left = text_before(&Cursor);
			if (right == '\n')
				linechanged = true;

			if (word_boundary(left, right))
				break;
		}

		if (linechanged)
		{
			CurrentLine = Cursor;
			text_line_start(&CurrentLine);
		}
	}
}

void cursor_wordright(uint16_t count)
{
	while (count--)
	{
		while (!text_is_end(&Cursor))
		{
			uint16_t left = text_get(&Cursor);
			uint16_t right;

			text_next(&Cursor);
			right = text_get(&Cursor);
			if (left == '\n')
				CurrentLine = Cursor;

			if (word_boundary(left, right))
				break;
		}
	}
}

void insert_newline(void)
{
	if (text_insert_char('\n'))
	{
		l3_goto(0, CurrentLineY);
		draw_line(&CurrentLine);
		CurrentLineY = L3ScreenY;
		DisplayHeight[CurrentLineY] = 0;
	}
}

void insert_mode(bool replacing)
{
	_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set,
					(replacing ? "Replace mode" : "Insert mode"));
	help_mode(replacing ? HELP_REPLACE : HELP_INSERT);

	for (;;)
	{
		uint16_t c = l3_getc();
		if (c == 0x07)             // EDIT
			break;
		if (c == 0x08 || c == 0x09 || c == 0x0A || c == 0x0B)
		{
			// Cursor keys: LEFT, RIGHT, DOWN, UP
			if (c == 0x08)
				cursor_left(1);
			else if (c == 0x09)
				cursor_right(1);
			else if (c == 0x0A)
				cursor_down(1);
			else
				cursor_up(1);
			recompute_screen_position();
			continue;
		}
		if (c < 32 && c != 13 && c != 0x0C)
			continue;              // other control codes are not inserted

		EvilDirtyFlag = true;
		if (c == 0x0C)             // DELETE (technically backspace)
		{
			if (!text_eq(&Cursor, &CurrentLine))
			{
				text_prev(&Cursor);
				text_delete(1);
			}
		}
		else
		{
			// When the memory is full, nothing goes in
			if (replacing && !text_is_end(&Cursor) && (text_get(&Cursor) != '\n'))
				text_delete(1);

			if (c == 13)
				insert_newline();
			else
				text_insert_char(c);
		}

		redraw_current_line();
	}

	_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set,"");
}

void insert_text(uint16_t count)
{
	insert_mode(false);
}

void append_text(uint16_t count)
{
	cursor_end(1);
	recompute_screen_position();
	insert_text(count);
}

// With no count (UINT_MAX), to the end of the last line
void goto_line(uint16_t lineno)
{
	if (lineno == UINT_MAX)
	{
		text_end(&Cursor);
		CurrentLine = Cursor;
		text_line_start(&CurrentLine);
		return;
	}

	Cursor.blk = 0;
	Cursor.off = 0;
	CurrentLine = Cursor;

	while (!text_is_end(&Cursor) && --lineno)
	{
		text_line_end(&Cursor);
		if (text_is_end(&Cursor))
			break;
		text_next(&Cursor);
		CurrentLine = Cursor;
	}
}

// Deletes from the cursor up to, not including, end
static void delete_to(const struct TextPos* end)
{
	text_delete(text_distance(&Cursor, end));
}

void delete_right(uint16_t count)
{
	text_delete(count);

	redraw_current_line();
	EvilDirtyFlag = true;
}

void delete_rest_of_line(uint16_t count)
{
	static struct TextPos end;

	end = Cursor;
	text_line_end(&end);
	delete_to(&end);

	if (count != 0)
		redraw_current_line();
	EvilDirtyFlag = true;
}

// Deletes count lines, with their newlines, in one go
void delete_line(uint16_t count)
{
	static struct TextPos end;

	cursor_home(1);
	end = Cursor;
	while (count--)
	{
		text_line_end(&end);
		if (text_is_end(&end))
			break;
		text_next(&end);
		DisplayHeight[CurrentLineY] = 0;
	}
	delete_to(&end);

	redraw_current_line();
	EvilDirtyFlag = true;
}

void delete_word(uint16_t count)
{
	static struct TextPos end;

	end = Cursor;
	while (count--)
	{
		// Compare the character under the cursor with the next one, not the one before it
		uint16_t left = text_get(&end);

		while (!text_is_end(&end))
		{
			uint16_t right;

			text_next(&end);
			right = text_get(&end);

			if (text_is_end(&end) || (right == '\n'))
				break;
			if (word_boundary(left, right))
				break;

			left = right;
		}
	}
	delete_to(&end);

	redraw_current_line();
	EvilDirtyFlag = true;
}

void change_word(uint16_t count)
{
	delete_word(1);
	insert_text(count);
}

void change_rest_of_line(uint16_t count)
{
	delete_rest_of_line(1);
	insert_text(count);
}

void join(uint16_t count)
{
	static struct TextPos p;

	while (count--)
	{
		p = Cursor;
		text_line_end(&p);
		text_put(&p, ' ');
	}

	l3_goto(0, CurrentLineY);
	render_screen(&CurrentLine);
	EvilDirtyFlag = true;
}

void open_above(uint16_t count)
{
	cursor_home(1);
	if (!text_insert_char('\n'))
		return;
	text_prev(&Cursor);

	recompute_screen_position();
	l3_goto(0, CurrentLineY);
	render_screen(&CurrentLine);
	recompute_screen_position();

	insert_text(count);
}

void open_below(uint16_t count)
{
	cursor_down(1);
	open_above(count);
}

void replace_char(uint16_t count)
{
	uint16_t c;

	help_mode(HELP_REPLACE_CHAR);
	c = l3_getc();

	if (text_is_end(&Cursor))
		return;
	if (c == 13)
	{
		text_delete(1);
		/* The cursor ends up *after* the newline. */
		insert_newline();
		redraw_current_line();
		EvilDirtyFlag = true;
	}
	else if (isprint(c))
	{
		text_put(&Cursor, c);
		/* The cursor ends on *on* the replace character. */
		redraw_current_line();
		EvilDirtyFlag = true;
	}
}

void replace_line(uint16_t count)
{
	insert_mode(true);
}

void zed_save_and_quit(uint16_t count)
{
	if (!EvilDirtyFlag || save_file()) {
		quit();
	}

	// render_screen draws every text row in full, so the status row keeps the save's message
	l3_goto(0, 0);
	print_status = command_status_set;
	render_screen(&FirstLine);
}

void zed_force_quit(uint16_t count)
{
	quit();
}

void redraw_screen(uint16_t count)
{
	l3_clear();
	render_screen(&FirstLine);
}

void enter_delete_mode(uint16_t count)
{
	bindings = &DeleteBindings;
	OperatorCount = count;
}

void enter_zed_mode(uint16_t count)
{
	bindings = &ZedBindings;
	OperatorCount = count;
}

void enter_change_mode(uint16_t count)
{
	bindings = &ChangeBindings;
	OperatorCount = count;
}

const char normal_keys[] =
	LIBLAYER3_KEY_LEFT LIBLAYER3_KEY_DOWN LIBLAYER3_KEY_UP LIBLAYER3_KEY_RIGHT
	"^$hjklbwiAGxJOorR:\022dZc";

command_t* const normal_cbs[] =
{
	cursor_left,
	cursor_down,	
	cursor_up,		
	cursor_right,		
	cursor_home,
	cursor_end,	
	cursor_left,
	cursor_down,
	cursor_up,		
	cursor_right,
	cursor_wordleft,
	cursor_wordright,
	insert_text,
	append_text,	
	goto_line,
	delete_right,
	join,
	open_above, 
	open_below,  
	replace_char, 
	replace_line,  
	colon,         
	redraw_screen,	
	enter_delete_mode,
	enter_zed_mode,
	enter_change_mode
};

const struct bindings normal_bindings =
{
	NULL,
	normal_keys,
	normal_cbs,
	HELP_NORMAL
};

const char delete_keys[] = "dw$";
command_t* const delete_cbs[] =
{
	delete_line,   	  
	delete_word,         
	delete_rest_of_line,
};

const struct bindings DeleteBindings =
{
	"Delete",
	delete_keys,
	delete_cbs,
	HELP_DELETE
};

const char change_keys[] = "w$";
command_t* const change_cbs[] =
{
	change_word,         
	change_rest_of_line,
};

const struct bindings ChangeBindings =
{
	"Change",
	change_keys,
	change_cbs,
	HELP_CHANGE
};

const char zed_keys[] = "ZQ";
command_t* const zed_cbs[] =
{
	zed_save_and_quit, 
	zed_force_quit,	
};

const struct bindings ZedBindings =
{
	"Zed",
	zed_keys,
	zed_cbs,
	HELP_ZED
};

/* ======================================================================= */
/*                             COLON COMMANDS                              */
/* ======================================================================= */

void colon(uint16_t count)
{
    char c;

	print_status = command_status_print;

	memset(Buffer, 0, 128);
	char* w = Buffer;
	char* arg = 0;

	l3_goto(0, HEIGHT);
	l3_clear_to_eol();
	l3_putc(':');
	help_mode(HELP_COLON);

	for(;;) {
		c = l3_getc();
		if(c==0x0D)                         // enter
		break;

		if(c==0x0C && L3ScreenX >1) {          // backspace
			*w = 0;
			--L3ScreenX;
			l3_putc(' ');
			--L3ScreenX;
			--w;
		}
		else if(c>31 && (w - Buffer) < (sizeof(Buffer) - 2)) {
			l3_putc(c);
			*w = c;
			w++;
		}
	}

	// The status row now shows only what the command reports
	l3_goto(0, HEIGHT);
	l3_clear_to_eol();

	CommandPending = COMMAND_NONE;
	_far(BANK_COMMAND, (FarFn)command_parse);

	switch (CommandPending)
	{
		case COMMAND_SAVE:
		case COMMAND_SAVE_QUIT:
		{
			bool saved;
			if (CommandArg && FileName && stricmp(CommandArg, FileName))
			{
				// :w NAME writes to NAME; the text keeps its own name and stays changed
				char *fileName = FileName;
				bool dirty = EvilDirtyFlag;
				FileName = CommandArg;
				saved = save_file();
				FileName = fileName;
				EvilDirtyFlag = dirty;
			}
			else
			{
				// Text with no name takes NAME
				if (CommandArg && !FileName)
					_farWithPointer(BANK_COMMAND, (FarFnPointer)command_file_name_set, CommandArg);
				saved = save_file();
			}
			if (saved && CommandPending == COMMAND_SAVE_QUIT)
				quit();
			break;
		}

		case COMMAND_READ:
		case COMMAND_LOAD:
		{
			// The name is in Buffer. FileName changes only when :e loads the file.
			char *fileName = FileName;
			FileName = CommandArg;
			if (CommandPending == COMMAND_READ)
				_farWithUChar(BANK_COMMAND, (FarFnUChar)command_file_insert, false);
			else if (load_file())
			{
				FileName = fileName;
				_farWithPointer(BANK_COMMAND, (FarFnPointer)command_file_name_set, CommandArg);
				EvilDirtyFlag = false;
				break;
			}
			FileName = fileName;
			break;
		}
	}

	// render_screen draws every text row in full, so the status row keeps the command's message
	l3_goto(0, 0);
	print_status = command_status_set;
	render_screen(&FirstLine);
}

/* ======================================================================= */
/*                            EDITOR OPERATIONS                            */
/* ======================================================================= */

void main(int argc, const char* argv[])
{
    intrinsic_di();
    /*
     * Store original paging configuration
     */
    OriginalMMU6 = ZXN_READ_REG(REG_MMU0 + 6);
    OriginalMMU7 = ZXN_READ_REG(REG_MMU0 + 7);

    /*
     * Initalise the hardware
     */
    _far(BANK_SYSTEM, (FarFn)system_init);

    // In main, not in a bank: IDE_BANK goes through M_P3DOS, which pages RAM 7 in at 0xC000 during the call
    text_init();

    l3_clear();

	print_status = command_status_set;

	strcpy(MessageBuffer, "EviL " EVIL_VER " (" EVIL_BUILD "), ");
	ultoa(text_free() - (uint32_t)TEXT_RESERVE * TEXT_BLOCK_SIZE, MessageBuffer + strlen(MessageBuffer), 10);
	strcat(MessageBuffer, " bytes free");
	_farWithPointer(BANK_COMMAND, (FarFnPointer)print_status, MessageBuffer);

	if(argc>1) {
		FileName = malloc(strlen(argv[1])+1);
	    strcpy(FileName, argv[1]);
	}

	// A file too big to load is not opened: :w must not write the empty text over it
	if (!load_file()) {
		free(FileName);
		FileName = 0;
		load_file();
	}
	l3_goto(0, 0);
	render_screen(&FirstLine);
	bindings = &normal_bindings;

    if(!FileName) {
        _far(BANK_SYSTEM, (FarFn)system_splash);
    }

//	zx_border(INK_GREEN);  while(1);
	CommandCount = 0;
	for (;;)
	{
		const char* cmdp = 0;
		unsigned int c;

		recompute_screen_position();
		help_mode(bindings->help);

		for (;;)
		{

			c = l3_getc();
            if (isdigit(c))
            {
				CommandCount = (CommandCount *10) + (c-'0');
                itoa(CommandCount, Buffer, 10);
                strcat(Buffer, " repeat");

				_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set, Buffer);
            }
            else
            {

				_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set,"");
                break;
            }
		}

		cmdp = strchr(bindings->keys, c);
		if (cmdp)
		{
			command_t* cmd = bindings->callbacks[cmdp - bindings->keys];
			uint16_t count = CommandCount;
			if (count == 0)
			{
				if (cmd == goto_line)
					count = UINT_MAX;
				else
					count = 1;
			}
			count *= OperatorCount;
			OperatorCount = 1;
			CommandCount = 0;

			bindings = &normal_bindings;

			_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set,"");
			cmd(count);
			if (bindings->name)
				_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set,bindings->name);
		}
		else
		{
			_farWithPointer(BANK_COMMAND, (FarFnPointer)command_status_set,"Unknown key");
			bindings = &normal_bindings;
			CommandCount = 0;
			OperatorCount = 1;
		}
	}
    exit(0);
}

void at_exit(void) {
    /*
     * Shutdown gracefully
     */
    text_release();
    _far(BANK_SYSTEM, (FarFn)system_exit);

    /*
     * Restore original paging configuration
     */
    ZXN_WRITE_MMU6(OriginalMMU6);
    ZXN_WRITE_MMU7(OriginalMMU7);
    intrinsic_ei();
}