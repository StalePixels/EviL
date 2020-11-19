//
// Created by D Rimron-Soutter on 15/11/2020.
//

#include "error.h"
#include "../common/ula.h"
#include "common.h"
#include "window.h"
#include <arch/zxn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *SpuiErrorTitleValue = {"Value Error"};

void spui_error(const char* Title) {
	struct SPUIWindow *WindowArgs = malloc(sizeof (struct SPUIWindow));
//	if(window_args) exit(0);
	WindowArgs->title = malloc(*Title);
	strcpy(WindowArgs->title, Title);
	WindowArgs->row = SPUI_ERROR_ROW;
	WindowArgs->col = SPUI_ERROR_COL;
	WindowArgs->width = SPUI_ERROR_WIDTH;
	WindowArgs->height = SPUI_ERROR_HEIGHT;
	spui_window(WindowArgs);
	free(WindowArgs->title);
	free(WindowArgs);
}


void spui_error_end() {
	in_wait_nokey();
	SpuiWindowRow = SpuiWindowRow + 2;
	printAtStr(SpuiWindowRow,SpuiWindowCol, " Any key to continue! ");
	spui_any_key();
	zx_cls(PAPER_MAGENTA|BRIGHT);
}