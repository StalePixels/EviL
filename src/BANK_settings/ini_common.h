//
// Created by D Rimron-Soutter on 3/11/2020.
//

#ifndef D3_INI_COMMON_H
#define D3_INI_COMMON_H

#include <stdlib.h>
#include <stdbool.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <errno.h>

#include "../common/memory.h"
#include "../common/file.h"

extern unsigned char IniIn, IniOut;
#define IniLine Buff512
extern char *IniKey;
extern char *IniValue;
extern uint8_t *errno_filter;
extern uint32_t IniChar;

bool ini_get_one(const char *Filename, const char *Key);
// returns if managed to get a line
bool ini_get_line(bool IncComments);
bool ini_set_one(const char *Filename, const char *Key, const char *Value);

void ini_file_close();

#endif //D3_INI_COMMON_H
