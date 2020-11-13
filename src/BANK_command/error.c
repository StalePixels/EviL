//
// Created by D Rimron-Soutter on 12/11/2020.
//

#include "error.h"
#include "status.h"

void command_error_no_filename(void)
{
	command_status_print("No filename set");
}

void command_error_document_not_saved(void)
{
	command_status_print("Document not saved (use ! to confirm)");
}
