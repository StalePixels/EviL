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

#include "common/memory.h"
#include "liblayer3/liblayer3.h"

#include "BANK_system/system.h"



#define PAGE_INIT               94 /*47*/
#define PAGE_INIT0              94
#define PAGE_INIT1              95

#define WIDTH SCREENWIDTH
#define HEIGHT (SCREENHEIGHT-1)

uint8_t top_page, btm_page, OriginalMMU6, OriginalMMU7, file_handle;

const char* file_name = 0;

uint16_t screenx = 0, screeny = 0;
uint16_t status_line_length;
void (*print_status)(const char*);

uint8_t* buffer_start;
uint8_t* gap_start;
uint8_t* gap_end;
uint8_t* buffer_end;
uint16_t dirty;

uint8_t* first_line; /* <= gap_start */
uint8_t* current_line; /* <= gap_start */
uint16_t current_line_y;
uint8_t display_height[HEIGHT];
uint16_t line_length[HEIGHT];

uint16_t command_count;
typedef void command_t(uint16_t);

struct bindings
{
	const char* name;
	const char* keys;
	command_t* const* callbacks;
};

const struct bindings* bindings;

extern const struct bindings deleteBindings;
extern const struct bindings zed_bindings;
extern const struct bindings change_bindings;

char buffer[128];
char message_buffer[128];
#define cpm_default_dma     (uint8_t*)buffer

extern void colon(uint16_t count);
extern void goto_line(uint16_t lineno);


/* ======================================================================= */
/*                                SCREEN DRAWING                           */
/* ======================================================================= */

void l3_puti(int i)
{
	itoa(i, buffer, 10);
	l3_puts(buffer);
}

void goto_status_line(void)
{
	l3_goto(0, HEIGHT);
}

void set_status_line(const char* message)
{
	uint16_t length = 0;

	uint8_t oldx = screenx, oldy = screeny;

	goto_status_line();
	l3_revon();
	for (;;)
	{
		uint16_t c = *message++;
		if (!c)
			break;
		l3_putc(c);
		length++;
	}
	l3_revoff();
	while (length < status_line_length)
	{
		l3_putc(' ');
		length++;
	}
	status_line_length = length;
	l3_goto(oldx, oldy);
}

/* ======================================================================= */
/*                              BUFFER MANAGEMENT                          */
/* ======================================================================= */

void new_file(void)
{
	gap_start = buffer_start;
	gap_end = buffer_end;

	first_line = current_line = buffer_start;
	dirty = true;
}

uint16_t compute_length(const uint8_t* inp, const uint8_t* endp, const uint8_t** nextp)
{
	static uint16_t xo;
	static uint16_t c;

	xo = 0;
	for (;;)
	{
		if (inp == endp)
			break;
		if (inp == gap_start)
			inp = gap_end;

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
	uint16_t starty = screeny;
	uint8_t* inp = startp;

	while (screeny != HEIGHT)
	{
		if (inp == gap_start)
		{
			inp = gap_end;
			startp += (gap_end - gap_start);
		}
		if (inp == buffer_end)
		{
			if (xo == 0)
				l3_puts("~");
			l3_clear_to_eol();
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

	display_height[starty] = (xo / WIDTH) + 1;
	line_length[starty] = inp - startp;

	return inp;
}

/* inp <= gap_start */
void render_screen(uint8_t* inp)
{
	unsigned i;
	for (i=screeny; i != HEIGHT; i++)
		display_height[i] = 0;

	while (screeny < HEIGHT)
	{
		if (inp == current_line)
			current_line_y = screeny;
		inp = draw_line(inp);
	}
}

void adjust_scroll_position(void)
{
	uint16_t total_height = 0;

	first_line = current_line;
	while (first_line != buffer_start)
	{
		uint8_t* line_start = first_line;
		const uint8_t* line_end = line_start--;
		while ((line_start != buffer_start) && (line_start[-1] != '\n'))
			line_start--;

		total_height += (compute_length(line_start, line_end, NULL) / WIDTH) + 1;
		if (total_height > (HEIGHT/2))
			break;
		first_line = line_start;
	}

	l3_goto(0, 0);
	render_screen(first_line);
}

void recompute_screen_position(void)
{
	const uint8_t* inp;
	uint16_t length;

	if (current_line < first_line)
		adjust_scroll_position();

	for (;;)
	{
		inp = first_line;
		current_line_y = 0;
		while (current_line_y < HEIGHT)
		{
			uint16_t height;

			if (inp == current_line)
				break;

			height = display_height[current_line_y];
			inp += line_length[current_line_y];

			current_line_y += height;
		}

		if ((current_line_y >= HEIGHT) ||
			((current_line_y + display_height[current_line_y]) > HEIGHT))
		{
			adjust_scroll_position();
		}
		else
			break;
	}

	length = compute_length(current_line, gap_start, NULL);
	l3_goto(length % WIDTH, current_line_y + (length / WIDTH));
}

void redraw_current_line(void)
{
	uint8_t* nextp;
    uint8_t oldheight;

	oldheight = display_height[current_line_y];
	l3_goto(0, current_line_y);
	nextp = draw_line(current_line);
	if (oldheight != display_height[current_line_y])
		render_screen(nextp);

	recompute_screen_position();
}

/* ======================================================================= */
/*                                LIFECYCLE                                */
/* ======================================================================= */
void insert_file(void)
{
	strcpy(message_buffer, "Reading ");
    strcat(message_buffer, file_name);
	print_status(message_buffer);

    errno = 0;
    file_handle = esxdos_f_open(file_name, ESXDOS_MODE_R);
    if(errno)
        goto error;

    for (;;)
	{
		uint8_t* inptr;

        uint8_t i = esxdos_f_read(file_handle, cpm_default_dma, 128);
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
				if (gap_start == gap_end)
				{
					print_status("Out of memory");
					goto done;
				}
				*gap_start++ = c;
			}
		}
	}

error:
    strcpy(message_buffer, "Could not read file ");
    strcat(message_buffer, file_name);
    strcat(message_buffer, " (errno:");
    itoa(errno, message_buffer+strlen(message_buffer), 10);
    strcat(message_buffer, ")");
	print_status(message_buffer);
done:
	esxdos_f_close(file_handle);
	dirty = true;
	return;
}

void load_file(void)
{
	new_file();
    if(file_name)
		insert_file();

	dirty = false;
	goto_line(1);
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
    file_handle = esxdos_f_open(fcb, ESXDOS_MODE_W | ESXDOS_MODE_CT);
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
            esx_f_write(file_handle, cpm_default_dma, 128);
            if(errno)
				goto error;
			outp = cpm_default_dma;
		}

		// special case to get around CPMs 128b block
		if ((inp == buffer_end) && !pushed && (outp != cpm_default_dma)) {
		    uint8_t b = outp - cpm_default_dma;

            esx_f_write(file_handle, cpm_default_dma, outp - cpm_default_dma);
            if(errno)
                goto error;
            outp = cpm_default_dma;
		}
	}

	dirty = false;
	esxdos_f_close(file_handle);
	return true;

error:
    esxdos_f_close(file_handle);
	return false;
}

bool save_file(void)
{
    const char tempfcb[] = "EVILTEMP.$$$";

    errno = 0;
    file_handle = esxdos_f_open(file_name, ESX_MODE_OPEN_CREAT_NOEXIST);
    if(errno == ESX_EEXIST)
        goto file_exists;

    esxdos_f_close(file_handle);
	if (!errno) {
		/* The file does not exist. */
        if (really_save_file(file_name)) {
            dirty = false;
            return true;
        }
    }

    strcpy(message_buffer, "Failed to save file (errno:");
    itoa(errno, message_buffer+strlen(message_buffer), 10);
    strcat(message_buffer, ")");
    print_status(message_buffer);
    _far(BANK_SYSTEM,system_beep);

    return false;

file_exists:
	/* Write to a temporary file. */

	esxdos_f_close(file_handle);
	if (really_save_file(tempfcb) == false)
		goto tempfile;

    strcpy(message_buffer, "Renaming ");
    strcat(message_buffer, tempfcb);
  	strcat(message_buffer, " to ");
    strcat(message_buffer, file_name);
    print_status(message_buffer);

    errno = 0;
	esxdos_f_unlink(file_name);
    if (errno)
        goto commit;
    esx_f_rename(tempfcb, file_name);
	if (errno)
		goto commit;

	return true;

tempfile:
    strcpy(message_buffer, "Cannot create EVILTEMP.$$$ file - it may exist (errno:");
    itoa(errno, message_buffer+strlen(message_buffer), 10);
    strcat(message_buffer, ")");
    print_status(message_buffer);
    _far(BANK_SYSTEM,system_beep);
	return false;

commit:
    strcpy(message_buffer, "Cannot commit file; your data may be in EVILTEMP.$$$ (errno:");
    itoa(errno, message_buffer+strlen(message_buffer), 10);
    strcat(message_buffer, ")");
    print_status(message_buffer);
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
	while (gap_start != current_line)
		*--gap_end = *--gap_start;
}

void cursor_end(uint16_t count)
{
	while ((gap_end != buffer_end) && (gap_end[0] != '\n'))
		*gap_start++ = *gap_end++;
}

void cursor_left(uint16_t count)
{
	while (count--)
	{
		if ((gap_start != buffer_start) && (gap_start[-1] != '\n')) {
		    *--gap_end = *--gap_start;
		}
	}
}

void cursor_right(uint16_t count)
{
	while (count--)
	{
		if ((gap_end != buffer_end) && (gap_end[0] != '\n'))
			*gap_start++ = *gap_end++;
	}
}

void cursor_down(uint16_t count)
{
	while (count--)
	{
		uint16_t offset = gap_start - current_line;
		cursor_end(1);
		if (gap_end == buffer_end)
			return;

		*gap_start++ = *gap_end++;
		current_line = gap_start;
		cursor_right(offset);
	}
}

void cursor_up(uint16_t count)
{
	while (count--)
	{
		uint16_t offset = gap_start - current_line;

		cursor_home(1);
		if (gap_start == buffer_start)
			return;

		do
			*--gap_end = *--gap_start;
		while ((gap_start != buffer_start) && (gap_start[-1] != '\n'));

		current_line = gap_start;
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

		while (gap_start != buffer_start)
		{
			uint16_t right = *--gap_start = *--gap_end;
			uint16_t left = gap_start[-1];
			if (right == '\n')
				linechanged = true;

			if (word_boundary(left, right))
				break;
		}

		if (linechanged)
		{
			current_line = gap_start;
			while ((current_line != buffer_start) && (current_line[-1] != '\n'))
				current_line--;
		}
	}
}

void cursor_wordright(uint16_t count)
{
	while (count--)
	{
		while (gap_end != buffer_end)
		{
			uint16_t left = *gap_start++ = *gap_end++;
			uint16_t right = *gap_end;
			if (left == '\n')
				current_line = gap_start;

			if (word_boundary(left, right))
				break;
		}
	}
}

void insert_newline(void)
{
	if (gap_start != gap_end)
	{
		*gap_start++ = '\n';
		l3_goto(0, current_line_y);
		current_line = draw_line(current_line);
		current_line_y = screeny;
		display_height[current_line_y] = 0;
	}
}

void insert_mode(bool replacing)
{
	set_status_line(replacing ? "Replace mode" : "Insert mode");

	for (;;)
	{
		uint16_t oldheight;
		uint8_t* nextp;
		uint16_t length;
		uint16_t c = l3_getc();
		if (c == 0x07)             // EDIT
			break;

		dirty = true;
		if (c == 0x0C)             // DELETE (technically backspace)
		{
			if (gap_start != current_line)
				gap_start--;
		}
		else if (gap_start == gap_end)
		{
			/* Do nothing, out of memory */
		}
		else
		{
			if (replacing && (gap_end != buffer_end) && (*gap_end != '\n'))
				gap_end++;

			if (c == 13)
				insert_newline();
			else
				*gap_start++ = c;
		}

		redraw_current_line();
	}

	set_status_line("");
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
	while (gap_start != buffer_start)
		*--gap_end = *--gap_start;
	current_line = buffer_start;

	while ((gap_end != buffer_end) && --lineno)
	{
		while (gap_end != buffer_end)
		{
			uint16_t c = *gap_start++ = *gap_end++;
			if (c == '\n')
			{
				current_line = gap_start;
				break;
			}
		}
	}
}

void delete_right(uint16_t count)
{
	while (count--)
	{
		if (gap_end == buffer_end)
			break;
		gap_end++;
	}

	redraw_current_line();
	dirty = true;
}

void delete_rest_of_line(uint16_t count)
{
	while ((gap_end != buffer_end) && (*++gap_end != '\n'))
		;

	if (count != 0)
		redraw_current_line();
	dirty = true;
}

void delete_line(uint16_t count)
{
	while (count--)
	{
		cursor_home(1);
		delete_rest_of_line(0);
		if (gap_end != buffer_end)
		{
			gap_end++;
			display_height[current_line_y] = 0;
		}
	}

	redraw_current_line();
	dirty = true;
}

void delete_word(uint16_t count)
{
	while (count--)
	{
		uint16_t left = (gap_start == buffer_start) ? '\n' : gap_start[-1];

		while (gap_end != buffer_end)
		{
			uint16_t right = *++gap_end;

			if ((gap_end == buffer_end) || (right == '\n'))
				break;
			if (word_boundary(left, right))
				break;

			left = right;
		}
	}

	redraw_current_line();
	dirty = true;
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
		uint8_t* ptr = gap_end;
		while ((ptr != buffer_end) && (*ptr != '\n'))
			ptr++;

		if (ptr != buffer_end)
			*ptr = ' ';
	}

	l3_goto(0, current_line_y);
	render_screen(current_line);
	dirty = true;
}

void open_above(uint16_t count)
{
	if (gap_start == gap_end)
		return;

	cursor_home(1);
	*--gap_end = '\n';

	recompute_screen_position();
	l3_goto(0, current_line_y);
	render_screen(current_line);
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

	if (gap_end == buffer_end)
		return;
	if (c == '\n')
	{
		gap_end++;
		/* The cursor ends up *after* the newline. */
		insert_newline();
	}
	else if (isprint(c))
	{
		*gap_end = c;
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
	if (!dirty)
		quit();
	if (!file_name)
	{
		set_status_line("No filename set");
		return;
	}
	if (save_file())
		quit();
}

void zed_force_quit(uint16_t count)
{
	quit();
}

void redraw_screen(uint16_t count)
{
	l3_clear();
	render_screen(first_line);
}

void enter_delete_mode(uint16_t count)
{
	bindings = &deleteBindings;
	command_count = count;
}

void enter_zed_mode(uint16_t count)
{
	bindings = &zed_bindings;
	command_count = count;
}

void enter_change_mode(uint16_t count)
{
	bindings = &change_bindings;
	command_count = count;
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

const struct bindings deleteBindings =
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

const struct bindings change_bindings =
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

const struct bindings zed_bindings =
{
	"Zed",
	zed_keys,
	zed_cbs
};

/* ======================================================================= */
/*                             COLON COMMANDS                              */
/* ======================================================================= */

void print_colon_status(const char* s)
{
    uint8_t oldx = screenx, oldy = screeny;
    screeny = HEIGHT - 1; screenx = 0;
    l3_clear_to_eol();
    l3_puts(s);
    screenx = oldx; screeny = oldy;
}

void set_current_filename(const char* f)
{
    free(file_name);
    file_name = malloc(strlen(f)+1);
    strcpy(file_name, f);
	dirty = true;
}

void print_no_filename(void)
{
    print_colon_status("No filename set");
}

void print_document_not_saved(void)
{
    print_colon_status("Document not saved (use ! to confirm)");
}

void colon(uint16_t count)
{
    char c;

	print_status = print_colon_status;

//	for (;;)
	{
	    memset(buffer, 0, 128);
		char* w = buffer;
		char* arg = 0;
		goto_status_line();
        l3_clear_to_eol();
		l3_putc(':');

		for(;;) {
            c = l3_getc();
            if(c==0x0D)                         // enter
                break;

            if(c==0x0C && screenx>1) {          // backspace
                *w = 0;
                --screenx;
                l3_putc(' ');
                --screenx;
                --w;
            }
            else if(c>31) {
                l3_putc(c);
                *w = c;
                w++;
            }
        }

        w = strtok(buffer, " ");
		if (!*w)
			goto cleanup;
        arg = strtok(NULL, " ");
		switch (*w)
		{

			case 'w':
			{
				bool quitting = (w[1] == 'q');
				if (arg)
					set_current_filename(arg);
				if (!file_name)
					print_no_filename();
				else if (save_file())
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
					char *backupfcb = malloc(sizeof(file_name));
					memcpy(&backupfcb, &file_name, sizeof(backupfcb));
                    file_name = arg;
					insert_file();
					memcpy(&file_name, &backupfcb, sizeof(backupfcb));
					free(backupfcb);
				}
				else
					print_no_filename();
				break;
			}
			case 'e':
			{
				if (!arg)
					print_no_filename();
				else if (dirty && (w[1] != '!'))
					print_document_not_saved();
				else
				{
					set_current_filename(arg);
					load_file();
				}
				break;
			}
			case 'n':
			{
				if (dirty && (w[1] != '!')) {
                    print_document_not_saved();
                }
				else {
					new_file();
					file_name = 0;
				}
				break;
			}
			case 'q':
			{
				if (!dirty || (w[1] == '!'))
					quit();
				else
					print_document_not_saved();
				break;
			}

			default:
                print_status("Unknown command");
		}
	}
    cleanup:

	l3_clear();
	print_status = set_status_line;
	render_screen(first_line);
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

	buffer_start = (void *) 0xC000;
    buffer_end = (void *) 0xFFFE;
	*buffer_end = '\n';
	print_status = set_status_line;

	itoa((uint16_t)(buffer_end - buffer_start), buffer, 10);
	strcat(buffer, " bytes free");
	print_status(buffer);

	if(argc>1) {
	    file_name = malloc(strlen(argv[1])+1);
	    strcpy(file_name, argv[1]);
	}

	load_file();
	l3_goto(0, 0);
	render_screen(first_line);
	bindings = &normal_bindings;


    if(!file_name) {
        _far(BANK_SYSTEM,system_splash);
    }

	command_count = 0;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
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
                command_count = (command_count*10) + (c-'0');
                itoa(command_count, buffer, 10);
                strcat(buffer, " repeat");
                set_status_line(buffer);
            }
            else
            {
                set_status_line("");
                break;
            }
		}

		cmdp = strchr(bindings->keys, c);
		if (cmdp)
		{
			command_t* cmd = bindings->callbacks[cmdp - bindings->keys];
			uint16_t count = command_count;
			if (count == 0)
			{
				if (cmd == goto_line)
					count = UINT_MAX;
				else
					count = 1;
			}
			command_count = 0;

			bindings = &normal_bindings;
			set_status_line("");
			cmd(count);
			if (bindings->name)
				set_status_line(bindings->name);
		}
		else
		{
			set_status_line("Unknown key");
			bindings = &normal_bindings;
			command_count = 0;
		}
	}
#pragma clang diagnostic pop
    exit(0);
}

void at_exit() {
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