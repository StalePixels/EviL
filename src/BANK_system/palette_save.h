//
// Created by D Rimron-Soutter on 20/11/2020.
//

#ifndef EVIL_PALETTE_SAVE_H
#define EVIL_PALETTE_SAVE_H

#include <stdint.h>

// x000xxxx is ULA first palette	   (0x00)
// x011xxxx is Tilemap first palette   (0x30)
void system_palette_save(uint8_t PaletteType, uint8_t *MemoryDestination);

#endif//EVIL_PALETTE_SAVE_H
