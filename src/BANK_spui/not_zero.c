//
// Created by D Rimron-Soutter on 18/11/2020.
//

#include "not_zero.h"
#include "common.h"
#include "../common/ula.h"
#include "error.h"
#include "window.h"
#include <arch/zxn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>

void spui_not_zero(void) {
	spui_error("Value Error");
	printAtStr(SPUI_ERROR_ROW+2,SPUI_ERROR_COL+1, "NonZero Value Expected");
	printAtStr(SPUI_ERROR_ROW+4,SPUI_ERROR_COL+1, " You must use a bigger");
	printAtStr(SPUI_ERROR_ROW+5,SPUI_ERROR_COL+1, "number, so the setting");
	printAtStr(SPUI_ERROR_ROW+6,SPUI_ERROR_COL+1, "remains unchanged.");
	in_wait_nokey();
	printAtStr(SPUI_ERROR_ROW+8,SPUI_ERROR_COL+1, " Any key to continue! ");
	spui_any_key();
	zx_cls(PAPER_MAGENTA|BRIGHT);
}