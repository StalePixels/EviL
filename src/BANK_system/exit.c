//
// Created by D Rimron-Soutter on 20/11/2020.
//

#include "exit.h"
#include "../common/evil.h"
#include "init.h"
#include "palette_restore.h"
#include "palettes.h"
#include "system.h"
#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>

void system_exit() {
	// Files
	esxdos_f_close(FileHandle);

	// Free buffers
	esx_ide_bank_free(0, EvilBufferBank1);
	esx_ide_bank_free(0, EvilBufferBank0);

	// Restore Palettes
	system_palette_restore(RPC_SELECT_TILEMAP_PALETTE_0, SystemBackupTilemapPalette);
	system_palette_restore(RPC_SELECT_ULA_PALETTE_0, SystemBackupULAPalette);

	// Clear ULA to default colours
	zx_cls(PAPER_WHITE|INK_BLACK);
	zx_border(INK_WHITE);

	// disable textmode
	ZXN_NEXTREG(0x6b, 0);                                    // disable tilemap

	// Restore Textmode tiles
	system_textmode_restore();

	// Finally, restore the original CPU speed
	ZXN_NEXTREGA(REG_TURBO_MODE, SystemOriginalSpeed);
}