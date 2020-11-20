//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "palette_apply.h"
#include "palette_set.h"
#include "palettes.h"

#include <arch/zxn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void palette_apply(uint8_t PaletteSettings[]) {
	printf("%d is %d\n", PaletteSettings[0], (uint16_t) PaletteSettings[1]);
	switch(PaletteSettings[0]) {
		case SYSTEM_PALETTE_DEFAULT_BACKGROUND:
			printf("BG");
			// Set the border too, special case
			system_palette_set(RPC_SELECT_ULA_PALETTE_0, 19, &PaletteSettings[1]);
		case SYSTEM_PALETTE_DEFAULT_FOREGROUND:
			printf("FG");
			uint16_t Palette9BitIndex = PaletteSettings[0]<<1;
			SystemTilemapPalette[Palette9BitIndex] = PaletteSettings[1];
			SystemTilemapPalette[Palette9BitIndex+1] = PaletteSettings[2];

			system_palette_set(RPC_SELECT_TILEMAP_PALETTE_0, PaletteSettings[0],
							   &SystemTilemapPalette[Palette9BitIndex]);
			break;
		default:
			break;
	}
	printf("\n");
}