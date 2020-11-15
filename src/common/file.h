//
// Created by D Rimron-Soutter on 12/11/2020.
//

#ifndef EVIL_FILE_H
#define EVIL_FILE_H

#include <stdbool.h>

extern const char* FileName;
const char tmp_filename[];

bool really_save_file(const char* fcb);

void file_new(void);
#endif//EVIL_FILE_H
