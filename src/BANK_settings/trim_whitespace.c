//
// Created by D Rimron-Soutter on 3/11/2020.
//

#include <string.h>
#include "trim_whitespace.h"

char *trim_whitespace(char *str)
{
    char *end;

    // Trim leading space
    while(*str==32) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && *end==32) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}