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

extern unsigned char IniIn;
#define IniLine Buff512
extern char *IniKey;
extern char *IniValue;

// returns if managed to get a line
bool ini_get_line(bool IncComments);

#endif //D3_INI_COMMON_H
