//
// Created by D Rimron-Soutter on 12/11/2020.
//

#ifndef EVIL_EVIL_H
#define EVIL_EVIL_H

#include <stdbool.h>
#include <stdlib.h>

#define cpm_default_dma     (uint8_t*)buffer

#define WIDTH SCREENWIDTH
#define HEIGHT (SCREENHEIGHT-1)

extern void (*print_status)(const char*);

extern uint8_t OriginalMMU6, OriginalMMU7;			// Memory Ops
extern uint8_t FileHandle;							// File Ops

extern bool EvilDirtyFlag;

extern uint8_t* buffer_start;
extern uint8_t* gap_start;
extern uint8_t* gap_end;
extern uint8_t* buffer_end;

extern char buffer[128];
extern char message_buffer[128];


extern uint8_t* first_line; /* <= gap_start */
extern uint8_t* current_line; /* <= gap_start */

// TODO - Migrate these to BANK_command
void quit(void);
void insert_file(void);
void load_file(void);
void load_file(void);

#endif//EVIL_EVIL_H
