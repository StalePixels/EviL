//
// Created by D Rimron-Soutter on 12/11/2020.
//

#ifndef EVIL_MEMORY_H
#define EVIL_MEMORY_H

#include <stdlib.h>

#define BANK_SYSTEM				(47<<1)
#define BANK_COMMAND 			(46<<1)
#define BANK_SPUI	 			(45<<1)
#define BANK_SETTINGS			(44<<1)
#define BANK_FONTS   			(40<<1)

extern unsigned char _z_page_table[];
extern char Buff512[512];

void *_far(uint8_t BANK, void *(*FnPtr)(void));
void *_farWithPointer(uint8_t BANK, void *(*FnPtr)(void *), void *DataAddr);
void *_farWithUChar(uint8_t BANK, void *(*FnPtr)(uint8_t), uint8_t Data);

#endif//EVIL_MEMORY_H
