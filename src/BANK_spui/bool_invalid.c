//
// Created by D Rimron-Soutter on 19/11/2020.
//

#include "bool_invalid.h"
#include "common.h"
#include "error.h"
#include <arch/zxn.h>
#include <stdio.h>
#include <input.h>

void spui_bool_invalid(const char*Setting) {
	spui_error("Toggle Value Error");
	printAtStr(SPUI_ERROR_ROW+2,SPUI_ERROR_COL+1, "Expecting a True/False");
	printAtStr(SPUI_ERROR_ROW+3,SPUI_ERROR_COL+1, "but got the following:");
	printAtStr(SPUI_ERROR_ROW+5,SPUI_ERROR_COL+1, Setting);
//	printAt(SPUI_ERROR_ROW+5,SPUI_ERROR_COL+1); printf("%.*s", SPUI_ERROR_WIDTH-2, Setting);
	printAtStr(SPUI_ERROR_ROW+6,SPUI_ERROR_COL+1, "Use: True/False or Y/N");
	in_wait_nokey();
	printAtStr(SPUI_ERROR_ROW+8,SPUI_ERROR_COL+1, " Any key to continue! ");
	spui_any_key();
	zx_cls(PAPER_MAGENTA|BRIGHT);
}