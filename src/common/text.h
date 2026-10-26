//
// The text: a list of 8K blocks in RAM pages that EviL takes from NextZXOS at startup.
// The block format and the rules every function keeps are at the top of text.c
//

#ifndef EVIL_TEXT_H
#define EVIL_TEXT_H

#include <stdbool.h>
#include <stdint.h>

#define TEXT_BLOCK_SIZE			8192
// 2MB of RAM is 224 pages of 8K
#define TEXT_MAX_BLOCKS			224
// A block with less text than this joins a neighbour, when the two fit in one block
#define TEXT_SPARSE				1024
// Pages a file read leaves free, so that typing and block splits still work when a read fills memory
#define TEXT_RESERVE			1

// A place in the text: byte off of block blk
struct TextPos {
	uint8_t blk;
	uint16_t off;
};

// The places that stay right when the text changes: functions that add or remove
// blocks, or move text between blocks, correct all three
extern struct TextPos TextMark[3];
#define Cursor					TextMark[0]
#define CurrentLine				TextMark[1]		// the start of the line Cursor is in
#define FirstLine				TextMark[2]		// the first line on the screen

extern uint8_t TextPages;

void text_init(void);
void text_release(void);
void text_clear(void);
uint32_t text_capacity(void);
uint32_t text_free(void);

uint8_t text_get(const struct TextPos *p);
void text_put(const struct TextPos *p, uint8_t c);
bool text_is_start(const struct TextPos *p);
bool text_is_end(const struct TextPos *p);
bool text_eq(const struct TextPos *a, const struct TextPos *b);
bool text_lt(const struct TextPos *a, const struct TextPos *b);
void text_next(struct TextPos *p);
void text_prev(struct TextPos *p);
uint8_t *text_span(const struct TextPos *p, uint16_t *n);
uint8_t text_before(const struct TextPos *p);
void text_advance(struct TextPos *p, uint16_t n);
uint32_t text_distance(const struct TextPos *from, const struct TextPos *to);
void text_end(struct TextPos *p);
void text_line_end(struct TextPos *p);
void text_line_start(struct TextPos *p);

bool text_insert(const uint8_t *src, uint16_t n);
bool text_insert_char(uint8_t c);
void text_delete(uint32_t n);

// BANK_command/text_file.c
bool text_read(uint8_t handle);
bool text_write(uint8_t handle);

// For the block code: text.c and BANK_command/text_file.c
#define TEXT_WINDOW				((uint8_t *)0xE000)
#define TEXT_MARKS				(sizeof(TextMark) / sizeof(TextMark[0]))

// In the dot command's own memory at 0x2000 (text_blocks.asm)
extern uint8_t TextPage[TEXT_MAX_BLOCKS];
extern uint16_t TextUsed[TEXT_MAX_BLOCKS];
extern uint8_t TextBlocks;

void text_map(uint8_t blk);
void text_fix(struct TextPos *p);
void text_block_add(uint8_t at);
void text_block_move(uint8_t b, uint16_t n);
void text_tidy(uint8_t b);

#endif//EVIL_TEXT_H
