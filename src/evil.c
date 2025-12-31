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

#include "common/evil.h"
#include "common/memory.h"
#include "common/file.h"
#include "liblayer3/liblayer3.h"

#include "BANK_command/parse.h"
#include "BANK_command/status.h"
#include "BANK_command/error.h"
#include "BANK_system/system.h"

uint8_t L3ScreenX = 0, L3ScreenY = 0;
uint8_t EvilBufferBank1, EvilBufferBank0, OriginalMMU6, OriginalMMU7, FileHandle;

void (*print_status)(const char*);

uint8_t* BufferStart;
uint8_t* GapStart;
uint8_t* GapEnd;
uint8_t* BufferEnd;
bool EvilDirtyFlag;

uint8_t* FirstLine; /* <= gap_start */
uint8_t* CurrentLine; /* <= gap_start */
uint16_t CurrentLineY;
uint8_t DisplayHeight[HEIGHT];
uint16_t LineLength[HEIGHT];

uint16_t CommandCount;
typedef void command_t(uint16_t);

struct bindings
{
	const char* name;
	const char* keys;
	command_t* const* callbacks;
};

const struct bindings* bindings;

extern const struct bindings DeleteBindings;
extern const struct bindings ZedBindings;
extern const struct bindings ChangeBindings;

char Buffer[128];
char MessageBuffer[128];

extern void colon(uint16_t count);
extern void goto_line(uint16_t lineno);

/* ======================================================================= */
/*                              BUFFER MANAGEMENT                          */
/* ======================================================================= */

uint16_t compute_length(const uint8_t* inp, const uint8_t* endp, const uint8_t** nextp)
{
	static uint16_t xo;
	static uint16_t c;

	xo = 0;
	for (;;)
	{
		if (inp == endp)
			break;
		if (inp == GapStart)
			inp = GapEnd;

		c = *inp++;
		if (c == '\n')
			break;
		if (c == '\t')
			xo = (xo + 8) & ~7;
		else if (c < 32)
			xo += 2;
		else
			xo++;
	}

	if (nextp)
		*nextp = inp;
	return xo;
}

uint8_t* draw_line(uint8_t* startp)
{
	uint16_t xo = 0;
	uint16_t c;
	uint16_t starty = L3ScreenY;
	uint8_t* inp = startp;


	while (L3ScreenY != HEIGHT)
	{
		if (inp == GapStart)
		{
			inp = GapEnd;
			startp += (GapEnd - GapStart);
		}
		if (inp == BufferEnd)
		{
			if (xo == 0 && L3ScreenY < HEIGHT-1) {
				l3_puts("~");
				l3_clear_to_eol();
			}
			l3_newline();
			break;
		}

		c = *inp++;
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
	LineLength[starty] = inp - startp;

	return inp;
}

/* inp <= gap_start */
void render_screen(uint8_t* inp)
{
	unsigned i;
	for (i= L3ScreenY; i != HEIGHT; i++)
		DisplayHeight[i] = 0;

	while (L3ScreenY < HEIGHT)
	{
		if (inp == CurrentLine)
			CurrentLineY = L3ScreenY;
		inp = draw_line(inp);
	}
}

void adjust_scroll_position(void)
{
	uint16_t total_height = 0;

	FirstLine = CurrentLine;
	while (FirstLine != BufferStart)
	{
		uint8_t* line_start = FirstLine;
		const uint8_t* line_end = line_start--;
		while ((line_start != BufferStart) && (line_start[-1] != '\n'))
			line_start--;

		total_height += (compute_length(line_start, line_end, NULL) / WIDTH) + 1;
		if (total_height > (HEIGHT/2))
			break;
		FirstLine = line_start;
	}

	l3_goto(0, 0);
	render_screen(FirstLine);
}

void recompute_screen_position(void)
{
	const uint8_t* inp;
	uint16_t length;

	if (CurrentLine < FirstLine)
		adjust_scroll_position();

	for (;;)
	{
		inp = FirstLine;
		CurrentLineY = 0;
		while (CurrentLineY < HEIGHT)
		{
			uint16_t height;

			if (inp == CurrentLine)
				break;

			height = DisplayHeight[CurrentLineY];
			inp += LineLength[CurrentLineY];

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

	length = compute_length(CurrentLine, GapStart, NULL);
	l3_goto(length % WIDTH, CurrentLineY + (length / WIDTH));

	tilemap[L3ScreenY][L3ScreenX].flags = tilemap[L3ScreenY][L3ScreenX].flags + 128;
}

void redraw_current_line(void)
{
	uint8_t* nextp;
    uint8_t oldheight;

	oldheight = DisplayHeight[CurrentLineY];
	l3_goto(0, CurrentLineY);
	nextp = draw_line(CurrentLine);
	if (oldheight != DisplayHeight[CurrentLineY])
		render_screen(nextp);

	recompute_screen_position();
}

/* ======================================================================= */
/*                                LIFECYCLE                                */
/* ======================================================================= */
void insert_file(void)
{
	strcpy(MessageBuffer, "Reading ");
    strcat(MessageBuffer, FileName);
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);

    errno = 0;
	FileHandle = esxdos_f_open(FileName, ESXDOS_MODE_R);
    if(errno)
        goto error;

    for (;;)
	{
		uint8_t* inptr;

        uint8_t i = esxdos_f_read(FileHandle, cpm_default_dma, 128);
		if (i == 0) /* EOF */
			goto done;
        if(errno)
			goto error;

		inptr = cpm_default_dma;
		while (inptr != (cpm_default_dma+i))
		{
			uint8_t c = *inptr++;
			if (c != '\r')
			{
				if (GapStart == GapEnd)
				{
					_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, "Out of memory");
					goto done;
				}
				*GapStart++ = c;
			}
		}
	}

error:
    strcpy(MessageBuffer, "Could not read file ");
    strcat(MessageBuffer, FileName);
    strcat(MessageBuffer, " (errno:");
    itoa(errno, MessageBuffer +strlen(MessageBuffer), 10);
    strcat(MessageBuffer, ")");
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);
done:
	esxdos_f_close(FileHandle);
	EvilDirtyFlag = true;
}

void load_file(void)
{
	file_new();
    if(FileName)
		insert_file();

	EvilDirtyFlag = false;
	goto_line(1);
}

bool save_file(void)
{
	if (!FileName) {
		_far(BANK_COMMAND,(void *)command_error_no_filename);
		return false;
	}

    const char tempfcb[] = "EVILTEMP.$$$";

    errno = 0;
	FileHandle = esxdos_f_open(FileName, ESX_MODE_OPEN_CREAT_NOEXIST);
    if(errno == ESX_EEXIST) {
		goto file_exists;
	}

    esxdos_f_close(FileHandle);
	if (!errno) {
		/* The file does not exist. */
        if (really_save_file(FileName)) {
            EvilDirtyFlag = false;
            return true;
        }
    }

    strcpy(MessageBuffer, "Failed to save file (errno:");
    itoa(errno, MessageBuffer +strlen(MessageBuffer), 10);
    strcat(MessageBuffer, ")");
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);

    _far(BANK_SYSTEM,system_beep);

    return false;

file_exists:
	/* Write to a temporary file. */

	esxdos_f_close(FileHandle);
	if (really_save_file(tempfcb) == false)
		goto tempfile;

    strcpy(MessageBuffer, "Renaming ");
    strcat(MessageBuffer, tempfcb);
  	strcat(MessageBuffer, " to ");
    strcat(MessageBuffer, FileName);
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);

    errno = 0;
	esxdos_f_unlink(FileName);
    if (errno)
        goto commit;
    esx_f_rename(tempfcb, FileName);
	if (errno)
		goto commit;

	return true;

tempfile:
    strcpy(MessageBuffer, "Cannot create EVILTEMP.$$$ file - it may exist (errno:");
    itoa(errno, MessageBuffer +strlen(MessageBuffer), 10);
    strcat(MessageBuffer, ")");
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);
    _far(BANK_SYSTEM,system_beep);
	return false;

commit:
    strcpy(MessageBuffer, "Cannot commit file; your data may be in EVILTEMP.$$$ (errno:");
    itoa(errno, MessageBuffer +strlen(MessageBuffer), 10);
    strcat(MessageBuffer, ")");
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);
    _far(BANK_SYSTEM,system_beep);
	return false;
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
	while (GapStart != CurrentLine)
		*--GapEnd = *--GapStart;
}

void cursor_end(uint16_t count)
{
	while ((GapEnd != BufferEnd) && (GapEnd[0] != '\n'))
		*GapStart++ = *GapEnd++;
}

void cursor_left(uint16_t count)
{
	while (count--)
	{
		if ((GapStart != BufferStart) && (GapStart[-1] != '\n')) {
		    *--GapEnd = *--GapStart;
		}
	}
}

void cursor_right(uint16_t count)
{
	while (count--)
	{
		if ((GapEnd != BufferEnd) && (GapEnd[0] != '\n'))
			*GapStart++ = *GapEnd++;
	}
}

void cursor_down(uint16_t count)
{
	while (count--)
	{
		uint16_t offset = GapStart - CurrentLine;
		cursor_end(1);
		if (GapEnd == BufferEnd)
			return;

		*GapStart++ = *GapEnd++;
		CurrentLine = GapStart;
		cursor_right(offset);
	}
}

void cursor_up(uint16_t count)
{
	while (count--)
	{
		uint16_t offset = GapStart - CurrentLine;

		cursor_home(1);
		if (GapStart == BufferStart)
			return;

		do
			*--GapEnd = *--GapStart;
		while ((GapStart != BufferStart) && (GapStart[-1] != '\n'));

		CurrentLine = GapStart;
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

		while (GapStart != BufferStart)
		{
			uint16_t right = *--GapStart = *--GapEnd;
			uint16_t left = GapStart[-1];
			if (right == '\n')
				linechanged = true;

			if (word_boundary(left, right))
				break;
		}

		if (linechanged)
		{
			CurrentLine = GapStart;
			while ((CurrentLine != BufferStart) && (CurrentLine[-1] != '\n'))
				CurrentLine--;
		}
	}
}

void cursor_wordright(uint16_t count)
{
	while (count--)
	{
		while (GapEnd != BufferEnd)
		{
			uint16_t left = *GapStart++ = *GapEnd++;
			uint16_t right = *GapEnd;
			if (left == '\n')
				CurrentLine = GapStart;

			if (word_boundary(left, right))
				break;
		}
	}
}

void insert_newline(void)
{
	if (GapStart != GapEnd)
	{
		*GapStart++ = '\n';
		l3_goto(0, CurrentLineY);
		CurrentLine = draw_line(CurrentLine);
		CurrentLineY = L3ScreenY;
		DisplayHeight[CurrentLineY] = 0;
	}
}

void insert_mode(bool replacing)
{
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set,
					(replacing ? "Replace mode" : "Insert mode"));

	for (;;)
	{
		uint16_t oldheight;
		uint8_t* nextp;
		uint16_t length;
		uint16_t c = l3_getc();
		if (c == 0x07)             // EDIT
			break;

		EvilDirtyFlag = true;
		if (c == 0x0C)             // DELETE (technically backspace)
		{
			if (GapStart != CurrentLine)
				GapStart--;
		}
		else if (GapStart == GapEnd)
		{
			/* Do nothing, out of memory */
		}
		else
		{
			if (replacing && (GapEnd != BufferEnd) && (*GapEnd != '\n'))
				GapEnd++;

			if (c == 13)
				insert_newline();
			else
				*GapStart++ = c;
		}

		redraw_current_line();
	}

	_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set,"");
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

void goto_line(uint16_t lineno)
{
	while (GapStart != BufferStart)
		*--GapEnd = *--GapStart;
	CurrentLine = BufferStart;

	while ((GapEnd != BufferEnd) && --lineno)
	{
		while (GapEnd != BufferEnd)
		{
			uint16_t c = *GapStart++ = *GapEnd++;
			if (c == '\n')
			{
				CurrentLine = GapStart;
				break;
			}
		}
	}
}

void delete_right(uint16_t count)
{
	while (count--)
	{
		if (GapEnd == BufferEnd)
			break;
		GapEnd++;
	}

	redraw_current_line();
	EvilDirtyFlag = true;
}

void delete_rest_of_line(uint16_t count)
{
	while ((GapEnd != BufferEnd) && (*++GapEnd != '\n'))
		;

	if (count != 0)
		redraw_current_line();
	EvilDirtyFlag = true;
}

void delete_line(uint16_t count)
{
	while (count--)
	{
		cursor_home(1);
		delete_rest_of_line(0);
		if (GapEnd != BufferEnd)
		{
			GapEnd++;
			DisplayHeight[CurrentLineY] = 0;
		}
	}

	redraw_current_line();
	EvilDirtyFlag = true;
}

void delete_word(uint16_t count)
{
	while (count--)
	{
		uint16_t left = (GapStart == BufferStart) ? '\n' : GapStart[-1];

		while (GapEnd != BufferEnd)
		{
			uint16_t right = *++GapEnd;

			if ((GapEnd == BufferEnd) || (right == '\n'))
				break;
			if (word_boundary(left, right))
				break;

			left = right;
		}
	}

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
	while (count--)
	{
		uint8_t* ptr = GapEnd;
		while ((ptr != BufferEnd) && (*ptr != '\n'))
			ptr++;

		if (ptr != BufferEnd)
			*ptr = ' ';
	}

	l3_goto(0, CurrentLineY);
	render_screen(CurrentLine);
	EvilDirtyFlag = true;
}

void open_above(uint16_t count)
{
	if (GapStart == GapEnd)
		return;

	cursor_home(1);
	*--GapEnd = '\n';

	recompute_screen_position();
	l3_goto(0, CurrentLineY);
	render_screen(CurrentLine);
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
	uint16_t c = l3_getc();

	if (GapEnd == BufferEnd)
		return;
	if (c == '\n')
	{
		GapEnd++;
		/* The cursor ends up *after* the newline. */
		insert_newline();
	}
	else if (isprint(c))
	{
		*GapEnd = c;
		/* The cursor ends on *on* the replace character. */
		redraw_current_line();
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

	l3_clear();
	print_status = command_status_set;
	render_screen(FirstLine);
}

void zed_force_quit(uint16_t count)
{
	quit();
}

void redraw_screen(uint16_t count)
{
	l3_clear();
	render_screen(FirstLine);
}

void enter_delete_mode(uint16_t count)
{
	bindings = &DeleteBindings;
	CommandCount = count;
}

void enter_zed_mode(uint16_t count)
{
	bindings = &ZedBindings;
	CommandCount = count;
}

void enter_change_mode(uint16_t count)
{
	bindings = &ChangeBindings;
	CommandCount = count;
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
	normal_cbs
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
	delete_cbs
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
	change_cbs
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
	zed_cbs
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
		else if(c>31) {
			l3_putc(c);
			*w = c;
			w++;
		}
	}

	_far(BANK_COMMAND, command_parse);

	l3_clear();
	print_status = command_status_set;
	render_screen(FirstLine);
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
    _far(BANK_SYSTEM,system_init);

    l3_clear();

	print_status = command_status_set;

	itoa((uint16_t)(BufferEnd - BufferStart), MessageBuffer, 10);
	strcat(MessageBuffer, " bytes free");
	_farWithPointer(BANK_COMMAND, (void *(*)(void *))print_status, MessageBuffer);

	if(argc>1) {
		FileName = malloc(strlen(argv[1])+1);
	    strcpy(FileName, argv[1]);
	}

	load_file();
	l3_goto(0, 0);
	render_screen(FirstLine);
	bindings = &normal_bindings;

    if(!FileName) {
        _far(BANK_SYSTEM,system_splash);
    }

//	zx_border(INK_GREEN);  while(1);
	CommandCount = 0;
	for (;;)
	{
		const char* cmdp = 0;
		unsigned int c;

		recompute_screen_position();

		for (;;)
		{

			c = l3_getc();
            if (isdigit(c))
            {
				CommandCount = (CommandCount *10) + (c-'0');
                itoa(CommandCount, Buffer, 10);
                strcat(Buffer, " repeat");

				_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set, Buffer);
            }
            else
            {

				_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set,"");
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
			CommandCount = 0;

			bindings = &normal_bindings;

			_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set,"");
			cmd(count);
			if (bindings->name)
				_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set,bindings->name);
		}
		else
		{
			_farWithPointer(BANK_COMMAND, (void *(*)(void *))command_status_set,"Unknown key");
			bindings = &normal_bindings;
			CommandCount = 0;
		}
	}
    exit(0);
}

void at_exit(void) {
    /*
     * Shutdown gracefully
     */
    _far(BANK_SYSTEM,system_exit);

    /*
     * Restore original paging configuration
     */
    ZXN_WRITE_MMU6(OriginalMMU6);
    ZXN_WRITE_MMU7(OriginalMMU7);
    intrinsic_ei();
}