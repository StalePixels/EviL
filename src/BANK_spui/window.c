//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "window.h"
#include "../common/ula.h"
#include "../liblayer3/textmode.h"
#include "common.h"
#include "line.h"
#include "triangle.h"

#include <arch/zx/esxdos.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <intrinsic.h>
#include <z80.h>

#include <arch/zxn.h>
#include <string.h>
#include <stdio.h>

uint8_t SpuiWindowRow, SpuiWindowCol;

void spui_window(const struct SPUIWindow *WindowArgs) {
	uint16_t Address;
// raise the ULA over the tileset



	uint8_t th = 4+WindowArgs->row+WindowArgs->height;
	uint8_t tw = (4+WindowArgs->col+WindowArgs->width)<<1;

	for(uint8_t ty=1+WindowArgs->row; ty<th; ++ty) {
		for(uint8_t tx=(3+WindowArgs->col)<<1; tx<tw; ++tx) {
			tilemap[ty][tx].flags |=  1;
		}
	}


	printBrightOn(); printFlashOff();
	printPaper(INK_BLACK); printInk(INK_WHITE);
	SpuiContentLength = strlen(WindowArgs->title)+1;
	SpuiPaddingLength = (WindowArgs->width < SpuiContentLength)
								? 0 : (WindowArgs->width) - SpuiContentLength;
	printAt(WindowArgs->row, WindowArgs->col);

	if(SpuiPaddingLength > 5) {
		SpuiPaddingLength = SpuiPaddingLength;
		printf (" %s%.*s\n", WindowArgs->title, SpuiPaddingLength, SpuiPadding);
		SpuiWindowCol = WindowArgs->col + WindowArgs->width - 6;
		printAt(WindowArgs->row, SpuiWindowCol);
		SpuiWindowRow = WindowArgs->row;
		spui_triangle(SpuiWindowRow,  SpuiWindowCol, PAPER_BLACK  | INK_RED    | BRIGHT);
		spui_triangle(SpuiWindowRow,++SpuiWindowCol, PAPER_RED    | INK_YELLOW | BRIGHT);
		spui_triangle(SpuiWindowRow,++SpuiWindowCol, PAPER_YELLOW | INK_GREEN  | BRIGHT);
		spui_triangle(SpuiWindowRow,++SpuiWindowCol, PAPER_GREEN  | INK_CYAN   | BRIGHT);
		spui_triangle(SpuiWindowRow,++SpuiWindowCol, PAPER_CYAN   | INK_BLACK  | BRIGHT);


	} else {
		printf (" %s%.*s\n", WindowArgs->title, SpuiPaddingLength, SpuiPadding);
	}

	// now the empty space
	printPaper(INK_WHITE); printInk(INK_BLACK);
	SpuiPaddingLength = WindowArgs->height-1;
	while(SpuiPaddingLength) {
		SpuiWindowRow = WindowArgs->row + SpuiPaddingLength;
		printAt(SpuiWindowRow, WindowArgs->col);
		printf("%.*s", (WindowArgs->width), SpuiPadding);
		SpuiWindowCol = WindowArgs->col;
		spui_line(SpuiWindowRow, WindowArgs->col, SPUI_LINE_LEFT);
		spui_line(SpuiWindowRow, WindowArgs->col+WindowArgs->width-1, SPUI_LINE_RIGHT);
		SpuiPaddingLength--;
	}
	SpuiPaddingLength = WindowArgs->height;
	SpuiWindowRow = WindowArgs->row + SpuiPaddingLength;
	printAt(SpuiWindowRow, WindowArgs->col);
	printf("%.*s", (WindowArgs->width), "________________________________");
	spui_line(SpuiWindowRow, WindowArgs->col, SPUI_LINE_LEFT_BOTTOM);
	spui_line(SpuiWindowRow, WindowArgs->col+WindowArgs->width-1, SPUI_LINE_RIGHT_BOTTOM);

	SpuiWindowRow = WindowArgs->row + 2;
	SpuiWindowCol = WindowArgs->col + 1;
}
