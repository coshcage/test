/* Standard library header.
 */
#ifndef _STD_H_
#define _STD_H_

#define NULL ((void *)0)

#include <stddef.h>
//typedef size_t unsigned long;
//typedef ptrdiff_t long;

typedef unsigned char UCHART, * PUCHAR;

typedef struct st_BLOCK_HEADER
{
	struct st_BLOCK_HEADER * pnext;
	PUCHAR pblock;
	size_t size;
} BLOCK_HEADER, * P_BLOCK_HEADER;

#define MEM_SIZ (8192) /* Alter this macro to control the size of a heap. */

P_BLOCK_HEADER mpkInitMemory(void);
void *         memcpy       (void *       dst, void *       src, size_t size);
void *         memset       (void *       s,   int          c,   size_t n);
void *         malloc       (size_t       size);
void           free         (void *       ptr);
void *         realloc      (void *       ptr, size_t       size);
void *         calloc       (size_t       n,   size_t       size);
int            memcmp       (const void * s1,  const void * s2,  size_t n);

#define memmove(dst, src, size) memcpy(dst, src, size)

void           srand        (unsigned int seed);
int            rand         (void);

#define BUFSIZ    128
#define UCHAR_MAX 0xFF
#define CHAR_BIT  8

#endif
