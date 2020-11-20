//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "common.h"

/* move these to a NextZXOS allocated memory bank, later */
unsigned char SystemShadowTilemap[sizeof(tilemap)];
unsigned char SystemShadowTiles[sizeof(tiles)];
unsigned char SystemShadowULAPalette[512];
unsigned char SystemShadowTilemapPalette[512];