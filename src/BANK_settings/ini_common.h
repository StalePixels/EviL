//
// Created by D Rimron-Soutter on 3/11/2020.
//

#ifndef D3_INI_COMMON_H
#define D3_INI_COMMON_H

#include <stdlib.h>
#include <stdbool.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

extern unsigned char ini_in, ini_out;
extern char ini_line[512];
extern char *ini_key;
extern char *ini_value;
extern char *tmp_filename[];
extern uint8_t *errno_filter;
extern uint32_t ini_char;

void ini_file_read(const char *filename);
void ini_file_write(const char *filename);

extern void *ini_callback();

bool ini_get_one(const char *filename, const char *key);
// returns if managed to get a line
bool ini_get_line(bool inc_comments);
bool ini_set_one(const char *filename, const char *key, const char *value);

void ini_file_close();

#endif //D3_INI_COMMON_H
