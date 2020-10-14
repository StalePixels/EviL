//
// Created by D Rimron-Soutter on 07/10/2020.
//

#ifndef CPMISH_TEXTMODE_H
#define CPMISH_TEXTMODE_H

#define TEXTMODE_ULA_BEHIND             0x00
#define TEXTMODE_ULA_FRONT              0x01

#define TEXTMODE_DEFAULT_COLOUR         0x04
#define TEXTMODE_REVERSE_COLOUR         0x84

struct __tilemap
{
    unsigned char tile;
    unsigned char flags;
};

extern struct __tilemap tilemap[32][80];

struct __tiles
{
    unsigned char bmp[8];           // each tile image is 8 bytes (64px@1bpp)
};

extern struct __tiles tiles[256];   //

extern unsigned char font[];

#endif //CPMISH_TEXTMODE_H
