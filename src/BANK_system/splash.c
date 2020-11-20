//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "splash.h"
#include "../liblayer3/liblayer3.h"
#include <stdint.h>

void system_splash(void) {
	uint16_t oldx = L3ScreenX;
	uint16_t oldy = L3ScreenY;
	L3ScreenX = 12;
	L3ScreenY = 13;
	l3_puts("EviL a.k.a. Essentially vi Lite - a vi adjacent for NextZXOS");
	L3ScreenX = 14;
	L3ScreenY = 14;
	l3_puts("Originally forked from QE by David Given, part of CPMISH");
	L3ScreenX = 23;
	L3ScreenY = 17;
	l3_puts("Code: D. Rimron-Soutter, Stale Pixels");
	L3ScreenX = 26;
	L3ScreenY = 18;
	l3_puts(     "Fonts: Damien Guard, zx-origins");
	L3ScreenX = oldx;
	L3ScreenY = oldy;
}
