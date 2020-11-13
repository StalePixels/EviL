//
// Created by D Rimron-Soutter on 12/11/2020.
//

#ifndef EVIL_MEMORY_H
#define EVIL_MEMORY_H

#include <stdlib.h>

#define BANK_SYSTEM				47
#define BANK_COMMAND 			46

extern unsigned char _z_page_table[];

void _far(uint8_t BANK, void (*FnPtr)(void));

#endif//EVIL_MEMORY_H
