//
// Created by D Rimron-Soutter on 12/11/2020.
//
#include <arch/zxn.h>

#include "error.h"
#include "status.h"
#include "../common/memory.h"
#include "../BANK_system/beep.h"

void command_error_no_filename(void)
{
	command_status_print("No filename set");

	_far(BANK_SYSTEM, system_beep);
}

void command_error_document_not_saved(void)
{
	command_status_print("Document not saved (use ! to confirm)");

	_far(BANK_SYSTEM, system_beep);
}
