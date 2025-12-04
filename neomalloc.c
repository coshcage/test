#include <stddef.h>  /* Using type size_t. */
#include <limits.h>  /* Using macro CHAR_BIT. */
#include <string.h>  /* Using function memcpy, memset. */
#include <stdbool.h> /* Boolean type and constants. */

/* [HEAP MEMORY DIAGRAM]
 * +=HEAP_HEADER=+
 * |size         *===>sizeof(chunk).=head note + free chunk + data + foot note.
 * |-------------|
 * |hshsiz:3     |
 * +=============+
 * |   POINTER   *>-------\    Big chunks.
 * |-------------|        V
 * |   POINTER   *-->NULL |
 * |-------------|        |
 * |   POINTER   *-->NULL |    Small chunks.
 * +=============+--------|--------\
 * |Head_note    |        V        |
 * +=FREE_CHUNK==+<-------/<--\    |
 * | p[FCP_PREV] *>-----------/    |
 * |-------------|            ^    |
 * | p[FCP_NEXT] *>-----------/    > This is a free chunk.
 * +=============+                 |
 * |    DATA     *==sizeof(size_t) |
 * |             |  *2             |
 * +=============+        /--------/
 * |Foot_note    |        |
 * +=============+--------/
 */

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned char * PUCHAR;
typedef unsigned char   UCHART;

enum en_FreeChunkPointer
{
	FCP_PREV = 0,
	FCP_NEXT = 1,
	FCP_MAX  = 2
};

#define USED false
#define FREE true

typedef struct st_FreeChunk
{
	struct st_FreeChunk * p[FCP_MAX];
} FREE_CHUNK, * P_FREE_CHUNK;

typedef struct st_HeapHeader
{
	size_t size;
	size_t hshsiz;
} HEAP_HEADER, * P_HEAP_HEADER;

#define MIN_CHUNK_SIZE (sizeof(size_t) * 2 + sizeof(FREE_CHUNK))
#define HEAP_BEGIN(ph) (sizeof(HEAP_HEADER) + (((P_HEAP_HEADER)(ph))->hshsiz * sizeof(P_HEAP_HEADER)))
#define ALIGN          (sizeof(size_t) * CHAR_BIT / 4)
#define MASK           (ALIGN - 1)
#define ASIZE(size)    (((size) & MASK) ? ((size) + ALIGN) & ~(size_t)MASK: (size))
#define HEAD_NOTE(pfc) (*(size_t *)((PUCHAR)(pfc) - sizeof(size_t)))
#define FOOT_NOTE(pfc) (*(size_t *)((PUCHAR)(pfc) + (HEAD_NOTE(pfc) & ~(size_t)MASK)))
#define FREE_MASK      ( (size_t)FREE)
#define USED_MASK      (~(size_t)USED - 1)

size_t _nmCLZ(size_t n)
{
#ifdef __GNUC__
	return __builtin_clz(n);
#endif
	register size_t i = (size_t)0x1 << (sizeof(size_t) * CHAR_BIT - 1), j = 0;
	while (i)
	{
		if (n & i)
			++j;
		i >>= 1;
	}
	return j;
}

P_FREE_CHUNK * _nmLocateHashTable(P_HEAP_HEADER ph, size_t i)
{
	if (ph->hshsiz >= i) /* Locate into hash table directly. */
		return &i[(P_FREE_CHUNK *)((PUCHAR)ph + sizeof(HEAP_HEADER))];
	else /* Locate to biggest one. */
		return &(ph->hshsiz - 1)[(P_FREE_CHUNK *)((PUCHAR)ph + sizeof(HEAP_HEADER))];
}

void _nmUnlinkChunk(P_HEAP_HEADER ph, P_FREE_CHUNK ptr)
{
	register P_FREE_CHUNK pfc = (P_FREE_CHUNK)ptr;
	register P_FREE_CHUNK * ppfc;

	ppfc = _nmLocateHashTable(ph, _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size));
	
	if (NULL != *ppfc)
	{
		do
		{
			if (ptr == pfc)
			{
				pfc->p[FCP_PREV]->p[FCP_NEXT] = pfc->p[FCP_NEXT];
				pfc->p[FCP_NEXT]->p[FCP_PREV] = pfc->p[FCP_PREV];
			
				if (pfc == *_nmLocateHashTable(ph, _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size))) /* Reach at linked list header.. */
					*_nmLocateHashTable(ph, _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size)) = NULL;
			}
			pfc = pfc->p[FCP_PREV];
		}
		while (pfc != *ppfc);
	}
}

P_HEAP_HEADER nmCreateHeap(void * pbase, size_t size, size_t hshsiz)
{
	P_FREE_CHUNK pfc;
	HEAP_HEADER hh;
	FREE_CHUNK fc;
	size_t t;
	
	if (NULL == pbase)
		return NULL;
		
	if (0 == hshsiz)
		return NULL;
	
	if (size < sizeof(HEAP_HEADER) + (hshsiz * sizeof(P_FREE_CHUNK)) + MIN_CHUNK_SIZE)
		return NULL;
		
	hh.size   = size - sizeof(HEAP_HEADER) + (hshsiz * sizeof(P_HEAP_HEADER));
	hh.size   = hh.size & ~(size_t)MASK;
	hh.hshsiz = hshsiz;
	
	/* Set heap header. */
	memcpy(pbase, &hh, sizeof(HEAP_HEADER));
	
	/* Clear hash table. */
	memset((PUCHAR)pbase + sizeof(HEAP_HEADER), 0, hshsiz * sizeof(size_t));
	
	/* Set one free chunk. */
	t = hh.size - (2 * sizeof(size_t));
	if (t > ASIZE(t))
		t = ASIZE(t);
	else
		t = t & ~(size_t)MASK;

	pfc = (P_FREE_CHUNK)((PUCHAR)pbase + HEAP_BEGIN(&hh) + sizeof(size_t));
	HEAD_NOTE(pfc) = FOOT_NOTE(pfc) = t;
	HEAD_NOTE(pfc) &= FREE_MASK;
	FOOT_NOTE(pfc) &= FREE_MASK;
	
	/* Set free chunk structure. */
	fc.p[FCP_PREV] = fc.p[FCP_NEXT] = pfc;
	memcpy((PUCHAR)pbase + sizeof(HEAP_HEADER), &fc, sizeof(FREE_CHUNK));
	
	/* Set hash table. */
	t = _nmCLZ(t) - _nmCLZ(hh.size);
	*_nmLocateHashTable(pbase, t) = pfc;
		
	return (P_HEAP_HEADER)pbase;
}

P_HEAP_HEADER nmExtendHeap(P_HEAP_HEADER ph, size_t sizincl)
{
	return NULL;
}

void * nmAllocHeap(P_HEAP_HEADER ph, size_t size)
{
	size_t j;
	P_FREE_CHUNK pfc;
	
	size = ASIZE(size);
	j = _nmCLZ(size) - _nmCLZ(ph->size);
	
	/* Definitely cannot allocate. */
	if (j > _nmCLZ(size))
		return NULL;
	
	/* Search hash table. */
	pfc = *_nmLocateHashTable(ph, j);
	
	/* Search for fit chunk. */
	while (pfc)
	{
		if (size > HEAD_NOTE(pfc))
			pfc = pfc->p[FCP_NEXT];
		else
			break;
	}
	
	if (NULL == pfc)
		return NULL; /* Cannot find a suitable chunk. */
	
	if (pfc) /* Cut one chunk. */
	{
		if (size == HEAD_NOTE(pfc) || size <= HEAD_NOTE(pfc) - MIN_CHUNK_SIZE)
		{	/* No need to split. */
			_nmUnlinkChunk(ph, pfc);
			
			/* Set used. */
			HEAD_NOTE(pfc) &= USED_MASK;
			FOOT_NOTE(pfc) &= USED_MASK;
			
			return pfc;
		}
		else
		{	/* Split. */
			register P_FREE_CHUNK * ppfc;
			register void * pt;
			register size_t i;
			i = HEAD_NOTE(pfc) - size;
			i &= ~(size_t)MASK;
			HEAD_NOTE(pfc) = FOOT_NOTE(pfc) = size;
			pt = pfc;
			pfc = (P_FREE_CHUNK)((PUCHAR)pfc + size + sizeof(size_t) * 2);
			HEAD_NOTE(pfc) &= FREE_MASK;
			FOOT_NOTE(pfc) &= FREE_MASK;
			
			/* Put new free chunk to linked list. */
			j = _nmCLZ(i) - _nmCLZ(ph->size);
			
			/* Search hash table. */
			ppfc = _nmLocateHashTable(ph, j);
			if (NULL != *ppfc)
			{
				pfc->p[FCP_NEXT] = *ppfc;
				pfc->p[FCP_PREV] = (*ppfc)->p[FCP_PREV];
				(*ppfc)->p[FCP_PREV] = pfc;
			}
			*ppfc = pfc;
			return pt;
		}
	}
	
	return NULL;
}

void nmFreeHeap(P_HEAP_HEADER ph, void * ptr)
{
	P_FREE_CHUNK pfc = (P_FREE_CHUNK)ptr;
	bool bbtm = FREE, bhed = FREE;
	size_t upcolsiz, upcolcnt;
	size_t lwcolsiz, lwcolcnt;
	register size_t chksiz;
	register size_t i;
	
	/* Get upper bound. */
	if ((PUCHAR)pfc - sizeof(size_t) == (PUCHAR)ph + HEAP_BEGIN(ph))
		bhed = USED;
	
	upcolsiz = 0;
	upcolcnt = 0;
	
	do
	{
		if (FREE == bhed && (FREE == !!(chksiz = (*(size_t *)((PUCHAR)pfc - 2 * sizeof(size_t)) & ~(size_t)MASK))))
		{
			pfc = (P_FREE_CHUNK)((PUCHAR)pfc - 2 * sizeof(size_t) - chksiz);
			upcolsiz += HEAD_NOTE(pfc) & ~(size_t)MASK;
			++upcolcnt;
			
			/* Unlink chunk. */
			_nmUnlinkChunk(ph, pfc);
		}
		else
			break;
		
		if ((PUCHAR)pfc - sizeof(size_t) == (PUCHAR)ph + HEAP_BEGIN(ph))
			bhed = USED;
	}
	while (USED != bhed);
	
	pfc = (P_FREE_CHUNK)ptr;
	
	/* Get lower bound. */
	chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;
	
	if ((PUCHAR)pfc + chksiz + sizeof(size_t) - (PUCHAR)ph == ph->size)
		bbtm = USED;
	
	lwcolsiz = 0;
	lwcolcnt = 0;
	
	while (FREE == bbtm && (FREE == (*(size_t *)((PUCHAR)pfc + chksiz + sizeof(size_t)) & ~(size_t)MASK)))
	{
		pfc = (P_FREE_CHUNK)((PUCHAR)pfc + chksiz + 2 * sizeof(size_t));
		chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;
		lwcolsiz += chksiz;
		++lwcolcnt;
		
		/* Unlink chunk. */
		_nmUnlinkChunk(ph, pfc);
	}
	
	/* Coalesce up and downward. */
	pfc = (P_FREE_CHUNK)ptr;
	
	chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;
	
	i = upcolsiz + sizeof(size_t) * upcolcnt;
	
	pfc = (P_FREE_CHUNK)((PUCHAR)pfc - i + sizeof(size_t));
	
	chksiz += i;
	
	chksiz += lwcolsiz + sizeof(size_t) * lwcolcnt;
	
	chksiz -= 2 * sizeof(size_t);
	
	HEAD_NOTE(pfc) = FOOT_NOTE(pfc) = chksiz;
	HEAD_NOTE(pfc) &= FREE_MASK;
	FOOT_NOTE(pfc) &= FREE_MASK;
	
	/* Put free chunk back into hash table or leave it alone. */
	if ((i = _nmCLZ(HEAD_NOTE(pfc) & ~(size_t)MASK) - _nmCLZ(ph->size)) > ph->hshsiz)
	{
		HEAD_NOTE(pfc) &= FREE_MASK;
		FOOT_NOTE(pfc) &= FREE_MASK;
	}
	else
	{
		register P_FREE_CHUNK * ppfc;
		ppfc = _nmLocateHashTable(ph, _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size));
		if (NULL != *ppfc)
		{
			pfc->p[FCP_NEXT] = *ppfc;
			pfc->p[FCP_PREV] = (*ppfc)->p[FCP_PREV];
			(*ppfc)->p[FCP_PREV] = pfc;
		}
		*ppfc = pfc;
	}
}

void * nmReallocHeap(P_HEAP_HEADER ph, void * ptr, size_t size);

