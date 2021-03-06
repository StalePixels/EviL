//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "font_invalid.h"
#include "../common/ula.h"
#include "common.h"
#include "error.h"
#include "window.h"
#include <arch/zxn.h>
#include <input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void spui_font_invalid(const char*FontName) {
	spui_error("Font Name Error");

	printAtStr(SpuiWindowRow,SpuiWindowCol, "Unknown Font Face Name");
	SpuiWindowRow = SpuiWindowRow + 2;
	printAt(SpuiWindowRow,SpuiWindowCol+1);
						printf("\"%.*s\"", SPUI_ERROR_WIDTH-4, FontName);
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "is not recognised. See");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "help for a valid list.");
	spui_error_end();
}