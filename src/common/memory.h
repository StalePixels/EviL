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

// zsdcc needs a typedef: it reads an inline cast such as (void *(*)(void *)) as a data pointer
typedef void *(*FarFn)(void);
typedef void *(*FarFnPointer)(void *);
typedef void *(*FarFnUChar)(uint8_t);

void *_far(uint8_t BANK, FarFn FnPtr);
void *_farWithPointer(uint8_t BANK, FarFnPointer FnPtr, void *DataAddr);
void *_farWithUChar(uint8_t BANK, FarFnUChar FnPtr, uint8_t Data);

#endif//EVIL_MEMORY_H
