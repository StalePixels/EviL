//
// Created by D Rimron-Soutter on 12/11/2020.
//

#ifndef EVIL_EVIL_H
#define EVIL_EVIL_H

#include <stdbool.h>
#include <stdlib.h>

#define WIDTH LIBLAYER3_SCREENWIDTH
#define HEIGHT (LIBLAYER3_SCREENHEIGHT-1)

extern void (*print_status)(const char*);

extern uint8_t OriginalMMU6, OriginalMMU7;			// Memory Ops
extern uint8_t FileHandle;							// File Ops

extern bool EvilDirtyFlag;

#define COMMAND_NONE			0
#define COMMAND_SAVE			1
#define COMMAND_SAVE_QUIT		2
#define COMMAND_READ			3
#define COMMAND_LOAD			4

extern uint8_t CommandPending;
extern char* CommandArg;

extern char Buffer[128];
extern char MessageBuffer[128];


extern uint8_t OriginalMMU6, OriginalMMU7, FileHandle;


void at_exit(void);

// TODO - Migrate these to BANK_command
void quit(void);
bool load_file(void);

#endif//EVIL_EVIL_H
