//
// Created by D Rimron-Soutter on 19/11/2020.
//

#include "bool_invalid.h"
#include "common.h"
#include "error.h"
#include "window.h"
#include <arch/zxn.h>
#include <input.h>
#include <stdio.h>

void spui_bool_invalid(const char*Setting) {
	spui_error(SpuiErrorTitleValue);
	printAtStr(SpuiWindowRow,SpuiWindowCol, "Boolean Value Required");
	SpuiWindowRow = SpuiWindowRow + 2;
	printAtStr(SpuiWindowRow,SpuiWindowCol, "The setting:");
	printAt(++SpuiWindowRow,SpuiWindowCol+1);
							printf("%.*s", SPUI_ERROR_WIDTH-3, Setting);
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "uses True/False or Y/N");
	spui_error_end();
}