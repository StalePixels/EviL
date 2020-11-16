//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "get.h"
#include <stdio.h>
#include <string.h>

#include "library.h"

uint8_t font_get(const char* font_name[]) {
	uint8_t font_id = FONT_COUNT;
	while(font_id--) {
		printf("%s = %s, %d\n",font_names[font_id], font_name,
			   stricmp(font_names[font_id], font_name));
		if(!stricmp(font_names[font_id], font_name))
			return font_id;
	}
	return -1;
}