; The block list of common/text.c, in the dot command's own 8K at 0x2000, which is always paged in.
; 224 is TEXT_MAX_BLOCKS in common/text.h.

SECTION bss_dot

PUBLIC _TextPage
PUBLIC _TextUsed

_TextPage:  defs 224        ; uint8_t, the page of each block
_TextUsed:  defs 224 * 2    ; uint16_t, the bytes of text in each block
