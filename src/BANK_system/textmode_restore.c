//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "textmode_restore.h"
#include "../liblayer3/textmode.h"
#include "common.h"
#include <string.h>

void system_textmode_restore() {
	memcpy(tiles, SystemShadowTiles, sizeof(tiles));
	memcpy(tilemap, SystemShadowTilemap, sizeof(tilemap));
}