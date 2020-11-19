//
// Created by D Rimron-Soutter on 19/11/2020.
//

#include "string_is_false.h"

#include <string.h>
#include <stdlib.h>


const char *spui_words_false[] = {
		"false",
		"f",
		"no",
		"n",
		"0"
};

bool string_is_false(const char *value) {
	uint8_t terms = SPUI_FALSE_WORDCOUNT;
	while(terms)
		if(!stricmp(value, spui_words_false[--terms])) {
			return true;
		}
	return false;
}