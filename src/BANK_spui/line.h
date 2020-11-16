//
// Created by D Rimron-Soutter on 16/11/2020.
//

#ifndef EVIL_SPUI_LINE_H
#define EVIL_SPUI_LINE_H

#include <stdint.h>

#define SPUI_LINE_LEFT 128
#define SPUI_LINE_LEFT_BOTTOM 129
#define SPUI_LINE_RIGHT  1
#define SPUI_LINE_RIGHT_BOTTOM  2

void spui_line(uint8_t row, uint8_t col, uint8_t style);

#endif//EVIL_SPUI_LINE_H
