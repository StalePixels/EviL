//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "textmode_save.h"
#include "../liblayer3/textmode.h"
#include "palettes.h"
#include <string.h>

void system_textmode_save() {
	memcpy(SystemShadowTiles,   tiles, sizeof(tiles));
	memcpy(SystemShadowTilemap,   tilemap, sizeof(tilemap));
}
