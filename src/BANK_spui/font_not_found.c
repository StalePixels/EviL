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

// The end of the path, the file name, fits in the window
static void spui_font_file_name(const char *FileName) {
	uint16_t Length = strlen(FileName);

	if (Length > SPUI_ERROR_WIDTH - 4)
		FileName = FileName + Length - (SPUI_ERROR_WIDTH - 4);
	printAt(SpuiWindowRow, SpuiWindowCol + 1);
	printf("\"%s\"", FileName);
}

// The file cannot be opened or read
void spui_font_not_found(const char *FileName) {
	spui_error("Font File Error");
	printAtStr(SpuiWindowRow,SpuiWindowCol, "Cannot Read Font File");
	SpuiWindowRow = SpuiWindowRow + 2;
	spui_font_file_name(FileName);
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "Check the path and the");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "name of the file.");
	spui_error_end();
}

// A font file is 768 bytes: characters 32 to 127, 8 bytes each
void spui_font_wrong_size(const char *FileName) {
	spui_error("Font File Error");
	printAtStr(SpuiWindowRow,SpuiWindowCol, "Wrong Font File Size");
	SpuiWindowRow = SpuiWindowRow + 2;
	spui_font_file_name(FileName);
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "A font file must be");
	printAtStr(++SpuiWindowRow,SpuiWindowCol, "exactly 768 bytes.");
	spui_error_end();
}
