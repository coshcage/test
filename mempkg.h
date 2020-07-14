/*
 * Name:        mempkg.h
 * Description: Memory package.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0714201755A0714201755L00032
 * License:     Public Domain.
 */
#ifndef _MEMPKG_H_
#define _MEMPKG_H_

#include <stddef.h>

typedef unsigned char UCHART, * PUCHAR;

typedef struct st_BLOCK_HEADER
{
	struct st_BLOCK_HEADER * pnext;
	PUCHAR pblock;
	size_t size;
} BLOCK_HEADER, * P_BLOCK_HEADER;

#define MEM_SIZ (1024)

void mpkInitMemory(void);
void * memcpy     (void * dst, void * src, size_t size);
void * memset     (void * s,   int    c,   size_t n);
void * malloc     (size_t size);
void   free       (void * ptr);
void * realloc    (void * ptr, size_t size);
void * calloc     (size_t n,   size_t size);

#endif
