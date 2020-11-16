//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "font_not_found.h"
#include "../common/ula.h"
#include "error.h"
#include "window.h"
#include <arch/zxn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void spui_font_not_found(const char*FontName) {
	spui_error("Font File Error");
	printAtStr(SPUI_ERROR_ROW+2,SPUI_ERROR_COL+1, " Feature Not Finished ");
	printAtStr(SPUI_ERROR_ROW+4,SPUI_ERROR_COL+1, "Loading external fonts");
	printAtStr(SPUI_ERROR_ROW+5,SPUI_ERROR_COL+1, "is on the roadmap, but");
	printAtStr(SPUI_ERROR_ROW+6,SPUI_ERROR_COL+1, "we're not there yet...");
	printAtStr(SPUI_ERROR_ROW+8,SPUI_ERROR_COL+1, " Any key to continue! ");
	printf("\x07");
	printf("\x07");
	printf("\x07");
	printf("\x07");
	zx_cls(PAPER_MAGENTA|BRIGHT);
}