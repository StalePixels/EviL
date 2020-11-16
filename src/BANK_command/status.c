//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "status.h"
#include <stdlib.h>
#include "../common/evil.h"
#include "../liblayer3/liblayer3.h"

uint8_t status_line_length = WIDTH;

void command_status_print(const char* message) {
//	_farWithPointer(BANK_SPUI, (void (*)(void *)) SPUI_error, message_buffer);
	uint8_t oldx = ScreenX, oldy = ScreenY;
	ScreenY = HEIGHT - 1; ScreenX = 0;
	l3_clear_to_eol();
	l3_puts(message);
	ScreenX = oldx; ScreenY = oldy;
}


void command_status_set(const char* message)
{
	uint8_t length = 0;

	uint8_t oldx = ScreenX, oldy = ScreenY;

	l3_goto(0, HEIGHT - 1);
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