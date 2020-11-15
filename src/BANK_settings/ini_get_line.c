//
// Created by D Rimron-Soutter on 3/11/2020.
//

#include "ini_common.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


// Return the next line in the open ini. Max Len 512char
bool ini_get_line(bool inc_comments) {
    char c;                     // Current char in line
    uint8_t r;                  // Number of chars read
    uint16_t ini_char = 0;      // Current pos in this line

    start_line:
    r = esxdos_f_read(ini_in, &c, 1);

    while(r) {
        if(!ini_char && (c==';' || c=='[') && !inc_comments) {
            while (r && c != 10) {
                r = esxdos_f_read(ini_in, &c, 1);
            }
            goto start_line;
        }
        if(c==10) {
            if (ini_char) {
                ini_line[ini_char] = 0;
                return true;
            }
            goto start_line;
        }
        ini_line[ini_char++] = c;

        r = esxdos_f_read(ini_in, &c, 1);
    }

    return false;
}