//
// The help screen: shows the keys of the current mode. It opens by itself at the : prompt
// and after d, c, Z and r; BREAK opens and closes it in any mode
//

#include "help.h"
#include "evil.h"
#include "memory.h"
#include "../liblayer3/liblayer3.h"
#include "../BANK_spui/help.h"

uint8_t HelpMode;
uint8_t HelpTop = HELP_CLOSED;
bool HelpEnabled = true;
bool HelpFooter = true;
// true while the window is open because a mode opened it, not BREAK
static bool HelpAuto = false;

extern void render_screen(uint8_t* inp);

// The help goes in the half of the screen the cursor is not in
static uint8_t help_top(void)
{
	return (L3ScreenY < (LIBLAYER3_SCREENHEIGHT/2)) ? HELP_ROWS : 0;
}

// The text is drawn here, in main: BANK_SPUI and the text buffer both use 0xC000
static void help_close(void)
{
	uint8_t x = L3ScreenX, y = L3ScreenY;

	_far(BANK_SPUI, (FarFn)spui_help_erase);
	HelpTop = HELP_CLOSED;
	HelpFooter = false;
	l3_goto(0, 0);
	render_screen(FirstLine);
	l3_goto(x, y);
}

// The editor draws its text over the help, so l3_getc draws the help again before every key
void help_update(void)
{
	if (HelpTop == HELP_CLOSED)
		return;

	if (!HelpEnabled)
	{
		help_close();
		return;
	}

	if (HelpTop != help_top())
	{
		// Moving the window is not closing it: keep the footer
		bool footer = HelpFooter;

		help_close();
		HelpFooter = footer;
		HelpTop = help_top();
	}

	_farWithUChar(BANK_SPUI, (FarFnUChar)spui_help, HelpMode);
}

// Called on every mode change and before every key in normal mode.
// A window a mode opened closes when EviL goes to a mode that does not open it (for example c, then insert)
void help_mode(uint8_t mode)
{
	HelpMode = mode;

	if (!HelpEnabled)
		return;

	// Only the modes that wait for the user to choose a key open the help by themselves.
	// Normal, insert and replace mode do not: there the user moves around or types.
	if (mode == HELP_DELETE || mode == HELP_CHANGE || mode == HELP_ZED ||
		mode == HELP_REPLACE_CHAR || mode == HELP_COLON)
	{
		if (HelpTop == HELP_CLOSED)
		{
			HelpTop = help_top();
			HelpAuto = true;
		}
	}
	else if (HelpAuto)
	{
		HelpAuto = false;
		if (HelpTop != HELP_CLOSED)
			help_close();
	}
}

void help_break(void)
{
	if (!HelpEnabled)
		return;

	HelpAuto = false;

	if (HelpTop == HELP_CLOSED)
		HelpTop = help_top();
	else
		help_close();
}
