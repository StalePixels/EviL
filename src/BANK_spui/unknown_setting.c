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
	printAt(SpuiWindowRow,SpuiWindowCol);
	if(strlen(Setting)<SPUI_ERROR_WIDTH-5) {
		if(Setting[strlen(Setting)-1]=='\n')
			printf(" \"%.*s\"", strlen(Setting)-1, Setting);
		else
			printf(" \"%s\"", Setting);
	}
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "is not recognised. See");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "help for a valid list.");
	spui_error_end();
}