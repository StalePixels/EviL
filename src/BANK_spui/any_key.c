//
// Created by D Rimron-Soutter on 16/11/2020.
//

#include "any_key.h"

#include <input.h>

void spui_any_key(void) {
	while(in_inkey()==0);
	while(in_inkey()!=0);
}