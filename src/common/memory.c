//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "memory.h"

#include <arch/zxn.h>

char Buff512[512];
/* ======================================================================= */
/*                                 BANKED CODE                             */
/* ======================================================================= */
void *_far(uint8_t BANK, void *(*FnPtr)(void)) {
	uint8_t BottomPage = ZXN_READ_REG(REG_MMU0 + 6);
	uint8_t TopPage = ZXN_READ_REG(REG_MMU0 + 7);

	ZXN_WRITE_MMU6(_z_page_table[BANK]);
	ZXN_WRITE_MMU7(_z_page_table[BANK+1]);

	void *ptr = FnPtr();

	ZXN_WRITE_MMU6(BottomPage);
	ZXN_WRITE_MMU7(TopPage);

	return ptr;
}

void *_farWithPointer(uint8_t BANK, void *(*FnPtr)(void *), void *DataAddr) {
	uint8_t BottomPage = ZXN_READ_REG(REG_MMU0 + 6);
	uint8_t TopPage = ZXN_READ_REG(REG_MMU0 + 7);

	ZXN_WRITE_MMU6(_z_page_table[BANK]);
	ZXN_WRITE_MMU7(_z_page_table[BANK+1]);

    void *ptr = FnPtr(DataAddr);

	ZXN_WRITE_MMU6(BottomPage);
	ZXN_WRITE_MMU7(TopPage);

    return ptr;
}

void *_farWithUChar(uint8_t BANK, void *(*FnPtr)(uint8_t), uint8_t Data) {
	uint8_t BottomPage = ZXN_READ_REG(REG_MMU0 + 6);
	uint8_t TopPage = ZXN_READ_REG(REG_MMU0 + 7);

	ZXN_WRITE_MMU6(_z_page_table[BANK]);
	ZXN_WRITE_MMU7(_z_page_table[BANK+1]);

    void *ptr = FnPtr(Data);

	ZXN_WRITE_MMU6(BottomPage);
	ZXN_WRITE_MMU7(TopPage);

    return ptr;
}
