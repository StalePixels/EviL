//
// Created by D Rimron-Soutter on 07/10/2020.
//

#ifndef EVIL_BANKED_H
#define EVIL_BANKED_H

extern uint16_t screenx;
extern uint16_t screeny;

#define WAIT_FOR_SCANLINE(line)         while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                        while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line)
void banked_init();
void banked_help();
void banked_exit();
void banked_beep();

#endif //EVIL_BANKED_H
