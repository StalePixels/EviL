//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "memory.h"

#include <arch/zxn.h>

/* ======================================================================= */
/*                                 BANKED CODE                             */
/* ======================================================================= */
void _far(uint8_t BANK, void (*FnPtr)(void)) {
	uint8_t TopPage = ZXN_READ_REG(REG_MMU0 + 6);
	uint8_t BottomPage = ZXN_READ_REG(REG_MMU0 + 7);

	BANK = BANK << 1;

	ZXN_WRITE_MMU6(_z_page_table[BANK]);
	ZXN_WRITE_MMU7(_z_page_table[BANK+1]);

	FnPtr();

	ZXN_WRITE_MMU6(BottomPage);
	ZXN_WRITE_MMU7(TopPage);
}
