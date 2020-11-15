//
// Created by D Rimron-Soutter on 06/11/2020.
//

#include "create_dir.h"
#include <z80.h>
#include <arch/zxn.h>
#include <intrinsic.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_dir(char *pathname) {
    char *p;
    int r;

    /* Strip trailing '/' */
    if (pathname[strlen(pathname) - 1] == '/')
        pathname[strlen(pathname) - 1] = '\0';

    /* Try creating the directory. */
    r = esx_f_mkdir(pathname);

    if(errno==18) return;
    if (r != 0) {
        /* On failure, try creating parent directory. */
        p = strrchr(pathname, '/');
        if (p != NULL) {
            *p = '\0';
            create_dir(pathname);
            *p = '/';
            r = esx_f_mkdir(pathname);
        }
    }
    if (r != 0)
        printf("Could not create directory %s\n", pathname);
}