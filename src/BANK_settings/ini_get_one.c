//
// Created by D Rimron-Soutter on 3/11/2020.
//

#include "ini_common.h"
#include "trim_whitespace.h"
#include <errno.h>
#include <stdio.h>

#include <ctype.h>
#include <string.h>

bool ini_get_one(const char *Filename, const char *Key) {
    IniValue = 0;
    IniIn = esxdos_f_open(Filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);

    uint8_t* allowed_errno = errno_filter;
    while(*allowed_errno && errno!=*allowed_errno) {
        allowed_errno++;
    }
    if(errno==*allowed_errno) goto pass;

    exit(errno);

    if(errno==__ESXDOS_ENOENT) return true;

    pass:
    while(ini_get_line(false)) {
        IniKey = trim_whitespace(strtok(IniLine, "="));
        if(!stricmp(IniKey, Key)) {
            IniValue = trim_whitespace(strtok(NULL, "\n"));
            goto done;
        }
    }

    done:
    esxdos_f_close(IniIn);

    return (IniValue ? true : false);
}