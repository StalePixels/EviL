//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "palette_set.h"
#include <arch/zxn.h>

void system_palette_set(uint8_t PaletteType, uint8_t PaletteIndex, uint8_t *PaletteValues) {
	ZXN_NEXTREGA(REG_PALETTE_CONTROL,  PaletteType);              		// 0x43 (67) => Palette Control
	ZXN_NEXTREGA(REG_PALETTE_INDEX,    PaletteIndex);					// 0x40 (64) => Palette Index
	ZXN_NEXTREGA(REG_PALETTE_VALUE_16, *PaletteValues);       			// 0x44 (68) => Palette Value (9 bit colour)
	ZXN_NEXTREGA(REG_PALETTE_VALUE_16, *(PaletteValues++));       		// 0x44 (68) => Palette Value (9 bit colour)

}