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
    ini_value = 0;
    bool entry_exists = false;
    bool file_exists = true;

    ini_in = esxdos_f_open(filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
    if(errno==5) {
        file_exists = false;
        errno = 0;
        ini_out = command_create_file(filename);
    }
    else if(!errno) {
        ini_out = command_create_file(tmp_filename);
    }
    if(errno) exit(errno);

    if(!file_exists) goto create_line;

    ini_tmp = malloc(513);

    while(ini_get_line(true)) {
        strcpy(ini_tmp, ini_line);

        if(!entry_exists) {
            ini_key = strtok(ini_tmp, "=");
            if (!strcmp(ini_key, key)) {
                ini_value = strtok(NULL, "\n");

                esxdos_f_write(ini_out, key, strlen(key));
                esxdos_f_write(ini_out, "=", 1);
                esxdos_f_write(ini_out, value, strlen(value));
                esxdos_f_write(ini_out, "\n", 1);

                entry_exists = true;
                goto done;
            }
        }

        esxdos_f_write(ini_out, &ini_line, strlen(ini_line));
        esxdos_f_write(ini_out, &"\n", 1);

        done:;
    }

    create_line:
    // Insert line, so we are new, or never found the entry.
    if(!ini_value) {
        esxdos_f_write(ini_out, key, strlen(key));
        esxdos_f_write(ini_out, "=", 1);
        esxdos_f_write(ini_out, value, strlen(value));
        esxdos_f_write(ini_out, "\n", 1);
        ini_key = key;
        ini_value = value;
    }

    esxdos_f_close(ini_out);

    if(file_exists) {
        esxdos_f_close(ini_in);

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