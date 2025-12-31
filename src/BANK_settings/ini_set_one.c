//
// Created by D Rimron-Soutter on 3/11/2020.
//

#include "ini_common.h"
#include "../common/create_file.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

static char *ini_tmp;
bool ini_set_one(const char *filename, const char *key, const char *value) {
    errno = 0;
    IniValue = 0;
    bool entry_exists = false;
    bool file_exists = true;

    IniIn = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if(errno==5) {
        file_exists = false;
        errno = 0;
        IniOut = command_create_file((char *)filename);
    }
    else if(!errno) {
        IniOut = command_create_file((char *)tmp_filename);
    }
    if(errno) exit(errno);

    if(!file_exists) goto create_line;

    ini_tmp = malloc(513);

    while(ini_get_line(true)) {
        strcpy(ini_tmp, IniLine);

        if(!entry_exists) {
            IniKey = strtok(ini_tmp, "=");
            if (!strcmp(IniKey, key)) {
                IniValue = strtok(NULL, "\n");

                esxdos_f_write(IniOut, (void *)key, strlen(key));
                esxdos_f_write(IniOut, "=", 1);
                esxdos_f_write(IniOut, (void *)value, strlen(value));
                esxdos_f_write(IniOut, "\n", 1);

                entry_exists = true;
                goto done;
            }
        }

        esxdos_f_write(IniOut, &IniLine, strlen(IniLine));
        esxdos_f_write(IniOut, &"\n", 1);

        done:;
    }

    create_line:
    // Insert line, so we are new, or never found the entry.
    if(!IniValue) {
        esxdos_f_write(IniOut, (void *)key, strlen(key));
        esxdos_f_write(IniOut, "=", 1);
        esxdos_f_write(IniOut, (void *)value, strlen(value));
        esxdos_f_write(IniOut, "\n", 1);
        IniKey = (char *)key;
        IniValue = (char *)value;
    }

    esxdos_f_close(IniOut);

    if(file_exists) {
        esxdos_f_close(IniIn);

        esxdos_f_unlink(filename);
        if (errno) {
            printf("UNLINK ERR: %s", filename);
            exit(errno);
        }

        esx_f_rename(tmp_filename, filename);
        if (errno) {
            printf("RENAME ERR: %s >> %s", tmp_filename, filename);
            exit(errno);
        }

        free(ini_tmp);
    }

    return (entry_exists);
}