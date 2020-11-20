
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>

#include <z80.h>
#include <arch/zxn.h>
#include <arch/zx/esxdos.h>
#include <arch/zxn/esxdos.h>
#include <errno.h>
#include <input.h>
#include <compress/zx7.h>

#include "../BANK_fonts/set.h"
#include "../BANK_settings/load.h"
#include "../BANK_settings/show_errors.h"
#include "../common/evil.h"
#include "../common/memory.h"
#include "../liblayer3/liblayer3.h"
#include "../liblayer3/textmode.h"
#include "palette_restore.h"
#include "palette_save.h"
#include "palette_set.h"
#include "palettes.h"
#include "system.h"
unsigned char SystemOriginalSpeed;

void system_init() {
    // Store CPU speed
	SystemOriginalSpeed = ZXN_READ_REG(REG_TURBO_MODE);

    // Set CPU speed to 28Mhz
    ZXN_NEXTREG(REG_TURBO_MODE, 3);

    // Ensure we clean up as we shut down...
    atexit(at_exit);

    // Get 16k of scratch RAM for text editing
	EvilBufferBank0 = esx_ide_bank_alloc(0);
	EvilBufferBank1 = esx_ide_bank_alloc(0);

    // We're going to trash this area for the Editor canvas, so let's back it up so we can restore it
    system_textmode_save();

	_farWithUChar(BANK_FONTS,font_set, 0);
	memset(0x5C00, 0, 8);               // Make val 0 also blank

	// 0x6E (110) R/W =>  Tilemap Base Address
    //  bits 7-6 = Read back as zero, write values ignored
    //  bits 5-0 = MSB of address of the tilemap in Bank 5
    ZXN_NEXTREG(0x6e, 0x6C);                                    // tilemap base address is 0x6C00

    // 0x6F (111) R/W => Tile Definitions Base Address
    //  bits 7-6 = Read back as zero, write values ignored
    //  bits 5-0 = MSB of address of tile definitions in Bank 5
    ZXN_NEXTREG(0x6f, 0x5C);                                    // base address 0x5c00
																// (vis.chars(32+) at 0x5D00)

    ZXN_NEXTREG(REG_GLOBAL_TRANSPARENCY_COLOR, 0xE3);
    ZXN_NEXTREG(REG_FALLBACK_COLOR, 0x00);

	system_palette_save(RPC_SELECT_TILEMAP_PALETTE_0, SystemBackupTilemapPalette);
	system_palette_restore(RPC_SELECT_TILEMAP_PALETTE_0, SystemTilemapPalette);
	system_palette_save(RPC_SELECT_ULA_PALETTE_0, SystemBackupULAPalette);

	// Force ULA bright magenta to be transparent version
	ZXN_NEXTREG(REG_PALETTE_INDEX, 27);
	ZXN_NEXTREG(REG_PALETTE_VALUE_8, 0xE3);

	system_palette_set(RPC_SELECT_ULA_PALETTE_0, 19, SystemTilemapPalette);

	zx_border(INK_MAGENTA);


	ZXN_NEXTREG(0x6b, /*0b11001000*/ 0xC8);                     // enable tilemap, 80x32 mode,
																// 1bit palette "textmode"
	// Setup the ULA for overlay UI
    zx_cls(INK_MAGENTA|PAPER_MAGENTA|BRIGHT);

	// Establish the memory buffer constraints
	BufferStart = (void *) 0xC000;
	BufferEnd = (void *) 0xFFFE;
	*BufferEnd = '\n';

	// Load the preferences
	_far(BANK_SETTINGS,settings_load);

	// Even if errors were
	_farWithUChar(BANK_SETTINGS, settings_show_errors, true);
}

