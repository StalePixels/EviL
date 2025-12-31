//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include <stdlib.h>

#include "status.h"

#include "../common/evil.h"
#include "../liblayer3/liblayer3.h"

#include "../BANK_system/system.h"

uint8_t status_line_length = WIDTH;

// this *DOES* set the entire bottom line to the rev colour
void command_status_print(const char* message) {
	uint8_t oldx = L3ScreenX, oldy = L3ScreenY;
	L3ScreenY = HEIGHT;
	L3ScreenX = 0;
	l3_clear_to_eol();
	l3_puts(message);
	L3ScreenX = oldx;
	L3ScreenY = oldy;
}


// this doesn't set the entire bottom line to the rev colour
void command_status_set(const char* message)
{
	uint8_t length = 0;

	uint8_t oldx = L3ScreenX, oldy = L3ScreenY;

	l3_goto(0, HEIGHT);
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