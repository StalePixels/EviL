//
// Created by D Rimron-Soutter on 15/11/2020.
//

#ifndef EVIL_FONT_LIBRARY_H
#define EVIL_FONT_LIBRARY_H

#include <stdlib.h>

// The fonts come first in font_library and font_names, then the aliases
#define FONT_FACES		18
#define FONT_COUNT 		(FONT_FACES+5)
extern const uint8_t* font_library[];
extern const char* font_names[];


extern const uint8_t 	font_sinclair[];
extern const uint8_t 	font_cinema[];
extern const uint8_t 	font_courier[];
extern const uint8_t 	font_envious[];
extern const uint8_t 	font_envious_bold[];
extern const uint8_t 	font_envious_italic[];
extern const uint8_t 	font_envious_serif[];
extern const uint8_t 	font_envious_serif_bold[];
extern const uint8_t 	font_magic5[];
extern const uint8_t 	font_magic5_bold[];
extern const uint8_t 	font_magnetic[];
extern const uint8_t 	font_minecraft[];
extern const uint8_t 	font_plotter[];
extern const uint8_t 	font_plotter_bold[];
extern const uint8_t 	font_plotter_condensed[];
extern const uint8_t 	font_ocra[];
extern const uint8_t 	font_ocrb[];
extern const uint8_t 	font_sixties[];


#endif//EVIL_FONT_LIBRARY_H
