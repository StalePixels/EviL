//
// Created by D Rimron-Soutter on 15/11/2020.
//

#ifndef EVIL_FONT_SET_H
#define EVIL_FONT_SET_H

#include <stdint.h>

// The tile definitions of characters 32 to 127, 8 bytes each: 768 bytes, as in a .ch8 file
#define FONT_DEFINITIONS	((uint8_t *)0x5D00)
#define FONT_SIZE			768
// FontCurrent when the font came from a file
#define FONT_NONE			255

// The bundled font or alias in use, or FONT_NONE
extern uint8_t FontCurrent;
// A copy of the font in use, to put it back
extern uint8_t FontBackup[FONT_SIZE];

void font_set(const uint8_t embedded_font_id);

#endif//EVIL_FONT_SET_H
