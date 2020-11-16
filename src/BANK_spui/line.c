//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "line.h"
#include <arch/zxn.h>

void spui_line(uint8_t row, uint8_t col, uint8_t spui_line) {
	uint8_t bottom = spui_line;
	uint8_t *addr = zx_cxy2saddr(col-1, row-1);

	if(spui_line==SPUI_LINE_RIGHT_BOTTOM || spui_line==SPUI_LINE_LEFT_BOTTOM) {
		bottom = 255;
		--spui_line;
	}
	*addr = spui_line;
	addr = addr + 256;
	*addr = spui_line;
	addr = addr + 256;
	*addr = spui_line;
	addr = addr + 256;
	*addr = spui_line;
	addr = addr + 256;
	*addr = spui_line;
	addr = addr + 256;
	*addr = spui_line;
	addr = addr + 256;
	*addr = spui_line;
	addr = addr + 256;
	*addr = bottom;
}
