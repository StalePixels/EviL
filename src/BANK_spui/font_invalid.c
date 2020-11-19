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
	printAtStr(SPUI_ERROR_ROW+2,SPUI_ERROR_COL+1, "Unknown Font Face Name");
	printAt(SPUI_ERROR_ROW+4,SPUI_ERROR_COL+1); printf("\"%.*s\"", SPUI_ERROR_WIDTH-4, FontName);
	printAtStr(SPUI_ERROR_ROW+5,SPUI_ERROR_COL+1, "is not recognised. See");
	printAtStr(SPUI_ERROR_ROW+6,SPUI_ERROR_COL+1, "help for a valid list.");
	in_wait_nokey();
	printAtStr(SPUI_ERROR_ROW+8,SPUI_ERROR_COL+1, " Any key to continue! ");
	spui_any_key();
	zx_cls(PAPER_MAGENTA|BRIGHT);
}