//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "palette_save.h"

#include <z80.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>

void system_palette_save(uint8_t PaletteType, uint8_t *MemoryDestination) {
	// Select ULA palette
	ZXN_NEXTREGA(REG_PALETTE_CONTROL, PaletteType);              		// 0x43 (67) => Palette Control
	uint8_t PaletteIndex = 0;
	uint16_t MemoryIndex;
	do {
		MemoryIndex = PaletteIndex<<1;

		ZXN_NEXTREGA(REG_PALETTE_INDEX, PaletteIndex);					// 0x40 (64) => Palette Index
		MemoryDestination[MemoryIndex] = ZXN_READ_REG(0x41);       		// 0x41 (65) => Palette Value (8 bit colour)
		MemoryDestination[++MemoryIndex] = ZXN_READ_REG(0x44);       	// 0x44 (68) => Palette Value (9 bit colour)
	} while (++PaletteIndex);

}