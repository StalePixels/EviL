//
// Created by D Rimron-Soutter on 19/11/2020.
//

#include "unknown_setting.h"
#include "../common/ula.h"
#include "common.h"
#include "error.h"
#include "window.h"
#include <arch/zxn.h>
#include <input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void spui_unknown_setting(const char*Setting) {
	spui_error("Setting Error");

	printAtStr(SpuiWindowRow,SpuiWindowCol, " Unknown Setting Name ");
	SpuiWindowRow = SpuiWindowRow + 2;
	printAt(SpuiWindowRow,SpuiWindowCol+1);
	printf("\"%.*s\"", SPUI_ERROR_WIDTH-4, Setting);
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "is not recognised. See");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "help for a valid list.");
	spui_error_end();
}