//
// Created by D Rimron-Soutter on 20/11/2020.
//

#ifndef EVIL_SYSTEM_PALETTES_H
#define EVIL_SYSTEM_PALETTES_H

#include "../liblayer3/textmode.h"
#include <stdint.h>

extern uint8_t SystemShadowTilemap[sizeof(tilemap)];
extern uint8_t SystemShadowTiles[sizeof(tiles)];
extern uint8_t SystemShadowULAPalette[512];
extern uint8_t SystemTilemapPalette[512];
extern uint8_t SystemShadowTilemapPalette[512];

#endif//EVIL_SYSTEM_PALETTES_H
