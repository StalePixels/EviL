//
// Created by D Rimron-Soutter on 19/11/2020.
//

#include "string_is_true.h"
#include <stdlib.h>
#include <string.h>

const char *spui_words_true[] = {
		"true",
		"t",
		"yes",
		"y",
		"1",
		"on"
};

bool string_is_true(const char *value) {
	uint8_t terms = SPUI_TRUE_WORDCOUNT;
	while(terms)
		if(!stricmp(value, spui_words_true[--terms])) {
			return true;
		}
	return false;
}