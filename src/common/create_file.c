//
// Created by D Rimron-Soutter on 06/11/2020.
//

#include <z80.h>
#include <arch/zxn.h>
#include <intrinsic.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "create_file.h"
#include "create_dir.h"

unsigned char command_create_file(char *pathname) {
    unsigned char file_out;
    file_out = esxdos_f_open(pathname, ESXDOS_MODE_W | ESXDOS_MODE_CT);
    if (file_out == 0) {
        /* Try creating parent dir and then creating file. */
        char *p = strrchr(pathname, '/');
        if (p != NULL) {
            *p = '\0';
            create_dir(pathname);
            *p = '/';
            file_out = esxdos_f_open(pathname, ESXDOS_MODE_W | ESXDOS_MODE_CT);
        }
    }
    return (file_out);
}
`