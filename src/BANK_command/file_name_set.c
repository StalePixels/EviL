//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "file_name_set.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "error.h"
#include "../common/evil.h"
#include "../common/file.h"
#include "../common/memory.h"


const char FileNameTemp[] = "EVILTEMP.$$$";

void command_file_name_set(const char* f)
{
	free(FileName);
	FileName = malloc(strlen(f)+1);
	strcpy(FileName, f);
	EvilDirtyFlag = true;
}

