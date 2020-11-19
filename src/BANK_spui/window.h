//
// Created by D Rimron-Soutter on 16/11/2020.
//

#ifndef EVIL_SPUI_WINDOW_H
#define EVIL_SPUI_WINDOW_H

#include <stdint.h>

extern uint8_t SpuiWindowRow, SpuiWindowCol;
struct SPUIWindow {
	uint8_t row;
	uint8_t col;
	uint8_t width;
	uint8_t height;
	char*   title;
};

void spui_window(const struct SPUIWindow *WindowArgs);

#endif//EVIL_SPUI_WINDOW_H
