//
// Loads a font from a .ch8 file
//

#ifndef EVIL_FONT_LOAD_H
#define EVIL_FONT_LOAD_H

#include <stdint.h>

// What font_load returns
#define FONT_LOAD_OK			0
#define FONT_LOAD_NOT_READ		1
#define FONT_LOAD_WRONG_SIZE	2

uint8_t font_load(const char *file_name);

#endif//EVIL_FONT_LOAD_H
