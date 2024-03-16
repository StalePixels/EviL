//
// Created by D Rimron-Soutter on 07/10/2020.
//

#ifndef EVIL_BANKED_SYSTEM_H
#define EVIL_BANKED_SYSTEM_H

#define WAIT_FOR_SCANLINE(line)         while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) == line); \
                                        while (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) != line)
void system_init(void);
void system_splash(void);
void system_exit(void);
void system_beep(void);
void system_textmode_save(void);
void system_textmode_restore(void);

#endif //EVIL_BANKED_SYSTEM_H