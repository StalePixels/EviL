//
// Created by D Rimron-Soutter on 15/11/2020.
//

#ifndef EVIL_SPUI_ERROR_H
#define EVIL_SPUI_ERROR_H

#include "../common/ula.h"

#define SPUI_ERROR_WIDTH	24
#define SPUI_ERROR_HEIGHT	10
#define SPUI_ERROR_ROW		(((ULA_HEIGHT-SPUI_ERROR_HEIGHT)/2)-1)
#define SPUI_ERROR_COL		(((ULA_WIDTH-SPUI_ERROR_WIDTH)/2)+1)

void spui_error(const char* FontName);

#endif//EVIL_SPUI_ERROR_H
