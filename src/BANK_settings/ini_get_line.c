//
// Created by D Rimron-Soutter on 3/11/2020.
//

#include "ini_common.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


// Return the next line in the open ini. Max Len 512char
bool ini_get_line(bool IncComments) {
    char C;                     // Current char in line
    uint8_t r;                  // Number of chars read
    uint16_t IniChar = 0;      // Current pos in this line

    start_line:
    r = esxdos_f_read(IniIn, &C, 1);

    while(r) {
        if(!IniChar && (C ==';' || C =='[') && !IncComments) {
            while (r && C != 10) {
                r = esxdos_f_read(IniIn, &C, 1);
            }
            goto start_line;
        }
        if(C ==10) {
            if (IniChar) {
                IniLine[IniChar] = 0;
                return true;
            }
            goto start_line;
        }
        IniLine[IniChar++] = C;

        r = esxdos_f_read(IniIn, &C, 1);
    }

    return false;
}