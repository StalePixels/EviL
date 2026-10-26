//
// The help screen: shows the keys of the current mode. It opens by itself at the : prompt
// and after d, c, Z and r; BREAK opens and closes it in any mode
//

#ifndef EVIL_HELP_H
#define EVIL_HELP_H

#include <stdbool.h>
#include <stdint.h>

// The modes, as the help screen knows them
#define HELP_NORMAL				0
#define HELP_DELETE				1
#define HELP_CHANGE				2
#define HELP_ZED				3
#define HELP_INSERT				4
#define HELP_REPLACE			5
#define HELP_REPLACE_CHAR		6
#define HELP_COLON				7

// HelpTop is the ULA row of the help window's title bar, or HELP_CLOSED
#define HELP_CLOSED				255
// The window is half of the ULA's 24 rows, in the half of the screen the cursor is not in
#define HELP_ROWS				12

extern uint8_t HelpMode;
extern uint8_t HelpTop;
extern bool HelpEnabled;
// true until the first help window of the session closes: only that window shows the footer
extern bool HelpFooter;

void help_mode(uint8_t mode);
void help_update(void);
void help_break(void);

#endif//EVIL_HELP_H
