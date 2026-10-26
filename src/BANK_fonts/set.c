//
// Created by D Rimron-Soutter on 15/11/2020.
//

#include <stdint.h>

#include <arch/zx/esxdos.h>
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <compress/zx7.h>
#include <errno.h>
#include <input.h>
#include <stdio.h>
#include <z80.h>

#include "set.h"
#include "library.h"

uint8_t FontCurrent;
uint8_t FontBackup[FONT_SIZE];

void font_set(const uint8_t embedded_font_id) {
	dzx7_standard(font_library[embedded_font_id], FONT_DEFINITIONS);
	FontCurrent = embedded_font_id;
}