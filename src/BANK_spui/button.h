//
// Created by D Rimron-Soutter on 16/11/2020.
//

#ifndef EVIL_SPUI_BUTTON_H
#define EVIL_SPUI_BUTTON_H

#include <stdint.h>
struct SPUIButton {
	uint8_t row;
	uint8_t col;
	uint8_t paper;
	uint8_t ink;
	uint8_t highlight;
	char    shortcut;
	char*   title;
};

#endif//EVIL_SPUI_BUTTON_H
