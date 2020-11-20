//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "palette_restore.h"
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <stdio.h>
#include <z80.h>

void system_palette_restore(uint8_t PaletteType, uint8_t *MemorySource) {
	// Select ULA palette
	ZXN_NEXTREGA(REG_PALETTE_CONTROL, PaletteType);              		// 0x43 (67) => Palette Control

	uint8_t PaletteIndex = 0;
	ZXN_NEXTREGA(REG_PALETTE_INDEX, PaletteIndex);              		// 0x43 (67) => Palette Control
	do {
		ZXN_NEXTREGA(0x44, *MemorySource);       			// 0x44 (68) => Palette Value (9 bit colour)
		ZXN_NEXTREGA(0x44, *(++MemorySource));       		// 0x44 (68) => Palette Value (9 bit colour)
		++MemorySource;
	} while (++PaletteIndex);
}
