//
// Created by D Rimron-Soutter on 15/11/2020.
//

#include <arch/zx/esxdos.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apply.h"
#include "ini_common.h"
#include "load.h"
#include "trim_whitespace.h"

const char settings_filename[] = "/SYS/EviL.RC";

void settings_load(void) {
	IniValue = 0;
	errno = 0;
	bool entry_exists = false;
	bool file_exists = true;

	IniIn = esxdos_f_open(settings_filename, ESXDOS_MODE_R | ESXDOS_MODE_OE);
	if(errno==__ESXDOS_ENOENT) {
		errno = 0;
		return;
	}

	while(ini_get_line(false)) {
		settings_apply(IniLine);
	}

	esxdos_f_close(IniIn);
}