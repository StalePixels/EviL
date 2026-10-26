//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#include <z80.h>
#include <intrinsic.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include "file.h"
#include "evil.h"

#include "../BANK_system/system.h"
#include "../common/file.h"
#include "../common/memory.h"
#include "text.h"
char* FileName = 0;

void file_new(void)
{
	text_clear();
	EvilDirtyFlag = true;
}
