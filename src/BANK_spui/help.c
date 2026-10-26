//
// The help screen: BREAK shows the keys of the current mode
//
// The ULA draws the frame and the title bar, as in SPUN's modals; the tilemap draws the text inside.
//

#include "help.h"
#include "../common/help.h"
#include "../common/ula.h"
#include "../liblayer3/textmode.h"
#include "common.h"
#include "line.h"
#include "triangle.h"

#include <arch/zxn.h>
#include <stdio.h>
#include <string.h>

// ULA cells: column x is tilemap columns 8 + 2x and 9 + 2x, row y is tilemap row 4 + y
#define SPUI_HELP_LEFT			1
#define SPUI_HELP_RIGHT			30
#define SPUI_HELP_WIDTH			(SPUI_HELP_RIGHT - SPUI_HELP_LEFT + 1)
#define SPUI_HELP_TILE_ROW		4
#define SPUI_HELP_TILE_COL		(8 + 2 * SPUI_HELP_LEFT)
#define SPUI_HELP_COLUMNS		(2 * SPUI_HELP_WIDTH)
#define SPUI_HELP_TEXT_COL		(SPUI_HELP_TILE_COL + 2)
#define SPUI_HELP_TEXT_WIDTH	(SPUI_HELP_COLUMNS - 4)
#define SPUI_HELP_LINES			9

// Tilemap palette entries 6 and 7 are black on white, 8 and 9 red on white (BANK_system/palettes.c)
#define SPUI_HELP_ATTR			(6 | TEXTMODE_ULA_FRONT)
#define SPUI_HELP_KEY_ATTR		(8 | TEXTMODE_ULA_FRONT)
// In the page text, \001 switches between the text and the key colour, and takes no space
#define SPUI_HELP_KEY			'\001'


struct SpuiHelpPage {
	const char *title;
	const char *lines[SPUI_HELP_LINES];
};

// In the order of the HELP_ modes in common/help.h
static const struct SpuiHelpPage SpuiHelpPages[] = {
	{
		"Normal mode",
		{
			0,
			"\001LEFT h\001  left    \001SYM+H\001 line start     \001i\001  insert",
			"\001DOWN j\001  down    \001$\001     line end       \001A\001  add at line end",
			"\001UP k\001    up      \001b\001     word back      \001o\001  open line below",
			"\001RIGHT l\001 right   \001w\001     next word      \001O\001  open line above",
			"\0010-9\001     count   \001G\001     line n or last \001R\001  overwrite mode",
			"\001:\001       command \001x\001     delete char    \001d\001  delete: dd dw d$",
			"\001EXT+R\001   redraw  \001J\001     join lines     \001c\001  change: cw c$",
			"                \001r\001     replace char   \001Z\001  quit: ZZ ZQ",
		}
	},
	{
		"Delete",
		{
			0,
			"\001d\001        delete the line. 3dd deletes 3 lines",
			"\001w\001        delete to the next word or the line end.",
			"         3dw deletes 3 words",
			"\001$\001        delete to the end of the line",
			0,
			"Other keys, except 0-9, go back to normal mode.",
			0,
			0,
		}
	},
	{
		"Change",
		{
			0,
			"\001w\001        delete to the next word or the line end,",
			"         then insert",
			"\001$\001        delete to the end of the line, then insert",
			0,
			"Other keys, except 0-9, go back to normal mode.",
			0,
			0,
			0,
		}
	},
	{
		"Zed",
		{
			0,
			"\001Z\001        save if changed, then quit. If the save",
			"         fails, EviL does not quit",
			"\001Q\001        quit without saving",
			0,
			"Other keys, except 0-9, go back to normal mode.",
			0,
			0,
			0,
		}
	},
	{
		"Insert mode",
		{
			0,
			"\001EDIT\001     back to normal mode",
			"\001DELETE\001   delete left, as far as the line start",
			"\001ENTER\001    new line",
			"\001Cursor\001   move the cursor",
			"Other keys type their character.",
			0,
			0,
			0,
		}
	},
	{
		"Replace mode",
		{
			0,
			"\001EDIT\001     back to normal mode",
			"\001DELETE\001   delete left, as far as the line start",
			"\001ENTER\001    new line",
			"\001Cursor\001   move the cursor",
			"Other keys type over the text, and add to it at",
			"the end of the line.",
			0,
			0,
		}
	},
	{
		"Replace character",
		{
			0,
			"Type a character to put it in place of the one under",
			"the cursor.",
			0,
			"\001ENTER\001    put a line break in its place",
			0,
			"Other keys go back to normal mode.",
			0,
			0,
		}
	},
	{
		"Command prompt",
		{
			"\001ENTER\001 runs the command. \001DELETE\001 deletes left.",
			"\001set a=b\001         change setting a to b",
			"\001w\001               save",
			"\001w name\001          set the file name; it does not save",
			"\001wq\001              save, then quit",
			"\001r name\001          read a file in at the cursor",
			"\001e name\001          edit a file; e! name loses changes",
			"\001n\001               new, empty text; n! loses changes",
			"\001q\001               quit; q! loses changes",
		}
	},
};

// The footer says what to type in the mode it is shown in
static const char SpuiHelpFooterColon[] = "Type set help=off to turn this help off";
static const char SpuiHelpFooter[] = "In normal mode, :set help=off turns this help off";

static void spui_help_blank(uint8_t Attr) {
	struct __tilemap *Cell;
	uint8_t Row, Col;

	for(Row = HelpTop + SPUI_HELP_TILE_ROW; Row != HelpTop + SPUI_HELP_TILE_ROW + HELP_ROWS; ++Row) {
		Cell = &tilemap[Row][SPUI_HELP_TILE_COL];
		for(Col = SPUI_HELP_COLUMNS; Col; --Col, ++Cell) {
			Cell->tile = ' ';
			Cell->flags = Attr;
		}
	}
}

static void spui_help_text(uint8_t Row, uint8_t Col, const char *Text) {
	struct __tilemap *Cell = &tilemap[Row][Col];
	uint8_t Attr = SPUI_HELP_ATTR;

	for(; *Text; ++Text) {
		if(*Text == SPUI_HELP_KEY) {
			Attr = (Attr == SPUI_HELP_ATTR) ? SPUI_HELP_KEY_ATTR : SPUI_HELP_ATTR;
			continue;
		}
		Cell->tile = *Text;
		Cell->flags = Attr;
		++Cell;
	}
}

// printAt, spui_line and spui_triangle count rows and columns from 1
static void spui_help_frame(const char *Title) {
	uint8_t Row = HelpTop + 1;
	uint8_t Bottom = HelpTop + HELP_ROWS;
	uint8_t Col = SPUI_HELP_LEFT + 1;

	printBrightOn(); printFlashOff();
	printPaper(INK_BLACK); printInk(INK_WHITE);
	printAt(Row, Col);
	SpuiPaddingLength = SPUI_HELP_WIDTH - 1 - strlen(Title);
	printf(" %s%.*s", Title, SpuiPaddingLength, SpuiPadding);

	Col = Col + SPUI_HELP_WIDTH - 6;
	spui_triangle(Row,   Col, PAPER_BLACK  | INK_RED    | BRIGHT);
	spui_triangle(Row, ++Col, PAPER_RED    | INK_YELLOW | BRIGHT);
	spui_triangle(Row, ++Col, PAPER_YELLOW | INK_GREEN  | BRIGHT);
	spui_triangle(Row, ++Col, PAPER_GREEN  | INK_CYAN   | BRIGHT);
	spui_triangle(Row, ++Col, PAPER_CYAN   | INK_BLACK  | BRIGHT);

	// The sides: the cells between them stay transparent, so the tilemap text shows
	while(++Row != Bottom) {
		*zx_cxy2aaddr(SPUI_HELP_LEFT, Row - 1) = PAPER_WHITE | INK_BLACK | BRIGHT;
		*zx_cxy2aaddr(SPUI_HELP_RIGHT, Row - 1) = PAPER_WHITE | INK_BLACK | BRIGHT;
		spui_line(Row, SPUI_HELP_LEFT + 1, SPUI_LINE_LEFT);
		spui_line(Row, SPUI_HELP_RIGHT + 1, SPUI_LINE_RIGHT);
	}

	printPaper(INK_WHITE); printInk(INK_BLACK);
	printAt(Bottom, SPUI_HELP_LEFT + 1);
	printf("%.*s", SPUI_HELP_WIDTH, "________________________________");
	spui_line(Bottom, SPUI_HELP_LEFT + 1, SPUI_LINE_LEFT_BOTTOM);
	spui_line(Bottom, SPUI_HELP_RIGHT + 1, SPUI_LINE_RIGHT_BOTTOM);
}

void spui_help(uint8_t Mode) {
	const struct SpuiHelpPage *Page = &SpuiHelpPages[Mode];
	uint8_t Row = HelpTop + SPUI_HELP_TILE_ROW + 1;
	uint8_t Line;

	spui_help_blank(SPUI_HELP_ATTR);
	for(Line = 0; Line != SPUI_HELP_LINES; ++Line, ++Row) {
		if(Page->lines[Line])
			spui_help_text(Row, SPUI_HELP_TEXT_COL, Page->lines[Line]);
	}
	if(HelpFooter) {
		const char *Footer = (Mode == HELP_COLON) ? SpuiHelpFooterColon : SpuiHelpFooter;
		spui_help_text(Row, SPUI_HELP_TEXT_COL + (SPUI_HELP_TEXT_WIDTH - strlen(Footer)) / 2, Footer);
	}
	spui_help_frame(Page->title);
}

void spui_help_erase(void) {
	spui_help_blank(TEXTMODE_DEFAULT_COLOUR);
	zx_cls(PAPER_MAGENTA|BRIGHT);
}
