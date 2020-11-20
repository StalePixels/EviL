//
// Created by D Rimron-Soutter on 12/11/2020.
//

#ifndef EVIL_EVIL_H
#define EVIL_EVIL_H

#include <stdbool.h>
#include <stdlib.h>

#define cpm_default_dma     (uint8_t*) Buffer

#define WIDTH LIBLAYER3_SCREENWIDTH
#define HEIGHT (LIBLAYER3_SCREENHEIGHT-1)

extern void (*print_status)(const char*);

extern uint8_t OriginalMMU6, OriginalMMU7;			// Memory Ops
extern uint8_t FileHandle;							// File Ops

extern bool EvilDirtyFlag;

extern uint8_t* BufferStart;
extern uint8_t* GapStart;
extern uint8_t* GapEnd;
extern uint8_t* BufferEnd;

extern char Buffer[128];
extern char MessageBuffer[128];


extern uint8_t* FirstLine; /* <= GapStart */
extern uint8_t* CurrentLine; /* <= GapStart */

extern uint8_t OriginalMMU6, OriginalMMU7, EvilBufferBank1, EvilBufferBank0, FileHandle;


void at_exit();

// TODO - Migrate these to BANK_command
void quit(void);
void insert_file(void);
void load_file(void);
void load_file(void);

#endif//EVIL_EVIL_H
