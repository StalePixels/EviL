//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "font_not_found.h"
#include "../common/ula.h"
#include "common.h"
#include "error.h"
#include "window.h"
#include <arch/zxn.h>
#include <input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void spui_font_not_found(const char*FontName) {
	spui_error("Font File Error");
	printAtStr(SpuiWindowRow,SpuiWindowCol, " Feature Not Finished ");
	SpuiWindowRow = SpuiWindowRow + 2;
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "Loading external fonts");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "is on the roadmap, but");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "we're not there yet...");
	spui_error_end();
}