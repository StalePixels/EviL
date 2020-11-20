//
// Created by D Rimron-Soutter on 20/11/2020.
//

#ifndef EVIL_SYSTEM_PALETTES_H
#define EVIL_SYSTEM_PALETTES_H

#define RPC_SELECT_TILEMAP_PALETTE_0 0x30
#define RPC_SELECT_ULA_TILEMAP_1		0x70


#include "../liblayer3/textmode.h"
#include <stdint.h>

extern uint8_t SystemShadowTilemap[sizeof(tilemap)];
extern uint8_t SystemShadowTiles[sizeof(tiles)];
extern uint8_t SystemBackupULAPalette[512];
extern uint8_t SystemTilemapPalette[512];
extern uint8_t SystemBackupTilemapPalette[512];

#endif//EVIL_SYSTEM_PALETTES_H
