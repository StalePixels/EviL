//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "beep.h"
#include "system.h"
#include <arch/zxn.h>
#include <stdio.h>

void system_beep() {
	zx_border(INK_RED);
	ZXN_NEXTREG(REG_TURBO_MODE, 0);
	printf("\x07");
	ZXN_NEXTREG(REG_TURBO_MODE, 3);
	for(uint8_t delay = 60; delay;delay--) {
		WAIT_FOR_SCANLINE(239);
	}
	zx_border(INK_BLACK);
}