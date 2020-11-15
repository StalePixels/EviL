//
// Created by D Rimron-Soutter on 3/11/2020.
//

#ifndef D3_TRIM_WHITESPACE_H
#define D3_TRIM_WHITESPACE_H
/*
 * https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 *
 * Note: This function returns a pointer to a substring of the original string.
 * If the given string was allocated dynamically, the caller must not overwrite
 * that pointer with the returned value, since the original pointer must be
 * deallocated using the same allocator with which it was allocated.  The return
 * value must NOT be deallocated using free() etc.
 *
 */
char *trim_whitespace(char *str);

#endif //D3_TRIM_WHITESPACE_H
