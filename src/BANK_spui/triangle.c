//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "triangle.h"
#include <z80.h>
#include <intrinsic.h>
#include <arch/zxn.h>
#include <arch/zx/esxdos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include <arch/zxn.h>
#include <string.h>
#include <stdio.h>

void spui_triangle(uint8_t row, uint8_t col, uint8_t attr) {
	uint8_t y = row-1;
	uint8_t x = col-1;
	uint8_t *addr;
	*zx_cxy2aaddr(x, y) = attr;
	addr = zx_cxy2saddr(x, y);
	*addr = 1;
	addr = addr + 256;
	*addr = 3;
	addr = addr + 256;
	*addr = 7;
	addr = addr + 256;
	*addr = 15;
	addr = addr + 256;
	*addr = 31;
	addr = addr + 256;
	*addr = 63;
	addr = addr + 256;
	*addr = 127;
	addr = addr + 256;
	*addr = 255;
}
