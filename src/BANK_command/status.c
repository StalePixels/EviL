//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "status.h"
#include <stdlib.h>
#include "../common/evil.h"
#include "../liblayer3/liblayer3.h"

void command_status_print(const char* s) {
	uint8_t oldx = ScreenX, oldy = ScreenY;
	ScreenY = HEIGHT - 1; ScreenX = 0;
	l3_clear_to_eol();
	l3_puts(s);
	ScreenX = oldx; ScreenY = oldy;
}