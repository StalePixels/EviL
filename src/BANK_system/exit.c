//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "exit.h"
#include "../common/evil.h"
#include "system.h"
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>

void system_exit() {
	zx_cls(PAPER_WHITE);
	zx_border(INK_WHITE);
	// Files
	esxdos_f_close(FileHandle);

	// Free buffers
	esx_ide_bank_free(0, top_page);
	esx_ide_bank_free(0, btm_page);

	// disable textmode
	ZXN_NEXTREG(0x6b, 0);                                    // disable tilemap

	// Restore Textmode tiles
	system_textmode_restore();

	// Finally, restore the original CPU speed
	ZXN_NEXTREGA(REG_TURBO_MODE, orig_cpu_speed);
}