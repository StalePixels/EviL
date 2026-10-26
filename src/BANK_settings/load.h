//
// Created by D Rimron-Soutter on 15/11/2020.
//

#ifndef EVIL_SETTINGS_LOAD_H
#define EVIL_SETTINGS_LOAD_H

#include <stdbool.h>

// true while the settings come from EviL.RC, false for :set
extern bool SettingsLoading;

void settings_load(void);

#endif//EVIL_SETTINGS_LOAD_H
