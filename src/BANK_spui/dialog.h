//
// Created by D Rimron-Soutter on 16/11/2020.
//

#ifndef EVIL_SPUI_DIALOG_H
#define EVIL_SPUI_DIALOG_H

#include "button.h"
#include <stdint.h>

struct SPUIDialog {
	uint8_t 				row;
	uint8_t 				col;
	uint8_t 				width;
	uint8_t 				height;
	char*   				title;
	struct SPUIButton*   	cancel_button;
	struct SPUIButton*   	action_button;
};


#endif//EVIL_SPUI_DIALOG_H
