//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "palette_apply.h"
#include "palette_set.h"
#include "palettes.h"

#include <arch/zxn.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void palette_apply(uint8_t PaletteSettings[]) {
	switch(PaletteSettings[0]) {
		case SYSTEM_PALETTE_DEFAULT_BACKGROUND:
			// Set the border too, special case
			system_palette_set(RPC_SELECT_ULA_PALETTE_0, 19, &PaletteSettings[1]);
		case SYSTEM_PALETTE_DEFAULT_FOREGROUND:
		case SYSTEM_PALETTE_CURSOR_FOREGROUND:
		case SYSTEM_PALETTE_CURSOR_BACKGROUND:
		case SYSTEM_PALETTE_STATUS_FOREGROUND:
		case SYSTEM_PALETTE_STATUS_BACKGROUND:
		case SYSTEM_PALETTE_ERROR_FOREGROUND:
		case SYSTEM_PALETTE_ERROR_BACKGROUND:
			// Generic palette settings
			uint16_t Palette9BitIndex;
			Palette9BitIndex = PaletteSettings[0] << 1;
			SystemTilemapPalette[Palette9BitIndex] = PaletteSettings[1];
			SystemTilemapPalette[Palette9BitIndex+1] = PaletteSettings[2];

			system_palette_set(RPC_SELECT_TILEMAP_PALETTE_0, PaletteSettings[0],
							   &SystemTilemapPalette[Palette9BitIndex]);
			break;
		default:
			break;
	}
}