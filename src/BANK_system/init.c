
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
#include "common.h"
#include "system.h"
unsigned char SystemOriginalSpeed;

uint8_t tilemap_background[16] = {
        0xE3,0x01,     // Transparent
        0xA0,0x00,     // Red
        0x14,0x00,     // Green
        0xAC,0x00,     // Yellow
        0x02,0x01,     // Blue
        0xA2,0x01,     // Magenta
        0x16,0x01,     // Cyan
        0xB6,0x01      // White
};

uint8_t tilemap_foreground[32] = {            // 0xE3 = 277
//      TRANS,     Red,       Green,     Yellow,    Blue,      Magenta,   Cyan,      White,
        0xE3,0x01, 0xA0,0x00, 0x14,0x00, 0xAC,0x00, 0x02,0x01, 0xA2,0x01, 0x16,0x01, 0xB6,0x01, // (normal)
        0x6D,0x01, 0xED,0x01, 0x7D,0x01, 0xFD,0x01, 0x77,0x01, 0xEE,0x01, 0x7F,0x01, 0xFF,0x01  // (bright)
};


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


	/*
	 * START BIT FOR SETTINGS HANDLER MIGRATION
	 */
	// Select ULA palette
    ZXN_NEXTREG(REG_PALETTE_CONTROL, 0x00);                     // 0x43 (67) => Palette Control,
																// x000xxxx is ULA first palette
    // Set Magenta back to proper E3
    ZXN_NEXTREGA(REG_PALETTE_INDEX, 27);
    ZXN_NEXTREGA(REG_PALETTE_VALUE_8, 0xE3);

    ZXN_NEXTREG(REG_PALETTE_CONTROL, 0x30);                     // 0x43 (67) => Palette Control
																// x011xxxx is Tilemap first palette
    ZXN_NEXTREG(REG_PALETTE_INDEX, 0);
    uint8_t i = 0;
    do {
//BG
        ZXN_NEXTREGA(0x44, tilemap_background[2*(i/32)]);       // 0x44 (68) => 9 bit colour)
																// autoinc after TWO writes
        ZXN_NEXTREGA(0x44, tilemap_background[2*(i/32)+1]);     // 0x44 (68) => 9 bit colour)
																// autoinc after TWO writes
//FG
        ZXN_NEXTREGA(0x44, tilemap_foreground[(i%32)]);         // 0x44 (68) => 9 bit colour)
																// autoinc after TWO writes
        ZXN_NEXTREGA(0x44, tilemap_foreground[(i%32)+1]);       // 0x44 (68) => 9 bit colour)
																// autoinc after TWO writes
    } while ((i = i + 2) != 0);


	/*
	 * END BIT FOR SETTINGS HANDLER MIGRATION
	 */

	zx_border(INK_BLACK);


	ZXN_NEXTREG(0x6b, /*0b11001000*/ 0xC8);                     // enable tilemap, 80x32 mode,
																// 1bit palette
	// Setup the ULA for overlay UI
    zx_cls(PAPER_MAGENTA|BRIGHT);

	// Establish the memory buffer constraints
	BufferStart = (void *) 0xC000;
	BufferEnd = (void *) 0xFFFE;
	*BufferEnd = '\n';

	// Load the preferences
	_far(BANK_SETTINGS,settings_load);

	// Even if errors were
	_farWithUChar(BANK_SETTINGS, settings_show_errors, true);
}

