//
// Created by D Rimron-Soutter on 16/11/2020.
//

#ifndef EVIL_SPUI_COMMON_H
#define EVIL_SPUI_COMMON_H

#include <stdint.h>
#include <input.h>

#define spui_any_key	in_wait_key();in_wait_nokey

extern uint8_t			SpuiPaddingLength;
extern uint8_t			SpuiContentLength;
extern const char 		*SpuiPadding;

#endif//EVIL_SPUI_COMMON_H
