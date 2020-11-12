//
// Created by D Rimron-Soutter on 07/10/2020.
//

#ifndef EVIL_BANKED_SYSTEM_H
#define EVIL_BANKED_SYSTEM_H

extern uint16_t screenx;
extern uint16_t screeny;

#define WAIT_FOR_SCANLINE(line)         while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                        while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line)
void system_init();
void system_splash();
void system_exit();
void system_beep();
void system_textmode_save();
void system_textmode_restore();

#endif //EVIL_BANKED_SYSTEM_H
