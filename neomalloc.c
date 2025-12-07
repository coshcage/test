#include <stddef.h>  /* Using type size_t. */
#include <limits.h>  /* Using macro CHAR_BIT. */
#include <string.h>  /* Using function memcpy, memset. */
#include <stdbool.h> /* Boolean type and constants. */

#ifdef _MSC_VER
#include <intrin.h>
#endif

/* [HEAP MEMORY DIAGRAM]
 * +=HEAP_HEADER=+
 * |size         *===>sizeof(chunk) == head note + free chunk + data + foot note.
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
 * |             |  *4             |
 * |             |                 |
 * |             |                 |
 * +=============+        /--------/
 * |Foot_note    |        |
 * +=============+--------/
 */

 /* Define constant NULL. */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* sizeof(UCHART) == 1. */
typedef unsigned char * PUCHAR;
typedef unsigned char   UCHART;

/* Chunk pointer indices. */
enum en_FreeChunkPointer
{
	FCP_PREV = 0,
	FCP_NEXT = 1,
	FCP_MAX = 2
};

/* Used and unused mark. */
#define USED false
#define FREE true

/* Free chunk linked list node. */
typedef struct st_FreeChunk
{
	struct st_FreeChunk * p[FCP_MAX];
} FREE_CHUNK, * P_FREE_CHUNK;

/* Heap header structure. */
typedef struct st_HeapHeader
{
	size_t size;
	size_t hshsiz;
} HEAP_HEADER, * P_HEAP_HEADER;

/* Usable macros. */
#define MIN_CHUNK_SIZE (sizeof(size_t) * 2 + sizeof(FREE_CHUNK))
#define HEAP_BEGIN(ph) (sizeof(HEAP_HEADER) + (((P_HEAP_HEADER)(ph))->hshsiz * sizeof(P_HEAP_HEADER)))
#define ALIGN          (sizeof(size_t) * CHAR_BIT / 4)
#define MASK           (ALIGN - 1)
#define ASIZE(size)    (((size) & MASK) ? ((size) + ALIGN) & ~(size_t)MASK: (size))
#define HEAD_NOTE(pfc) (*(size_t *)((PUCHAR)(pfc) - sizeof(size_t)))
#define FOOT_NOTE(pfc) (*(size_t *)((PUCHAR)(pfc) + (HEAD_NOTE(pfc) & ~(size_t)MASK)))
#define FREE_MASK      ( (size_t)FREE)
#define USED_MASK      (~(size_t)USED - 1)

/* File level function declarations. */
size_t         _nmCLZ(size_t n);
P_FREE_CHUNK * _nmLocateHashTable(P_HEAP_HEADER ph, size_t i);
void           _nmUnlinkChunk(P_HEAP_HEADER ph, P_FREE_CHUNK ptr);
void           _nmRestoreEntrance(P_FREE_CHUNK * ppfc, P_FREE_CHUNK pfc);
void *         _nmSplitChunk(P_HEAP_HEADER ph, P_FREE_CHUNK pfc, size_t size);
void           _nmPutChunk(P_HEAP_HEADER ph, P_FREE_CHUNK pfc);

size_t _nmCLZ(size_t n)
{
#ifdef __GNUC__
	if (sizeof(size_t) == sizeof(long))
		__builtin_clzl(n);
	else if (sizeof(size_t) == sizeof(long long))
		__builtin_clzll(n);
	else
		return __builtin_clz(n);

#elif defined _MSC_VER
	switch (sizeof(size_t) * CHAR_BIT)
	{
	case 16:
		return __lzcnt16(n);
	case 32:
		return __lzcnt(n);
#ifdef _M_X64
	case 64:
		return __lzcnt64(n);
#endif
	}
#endif
	register size_t i = n, j = 0;
	while (i)
	{
		i >>= 1;
		++j;
	}
	return sizeof(size_t) * CHAR_BIT - j;
}

P_FREE_CHUNK * _nmLocateHashTable(P_HEAP_HEADER ph, size_t i)
{
	if (ph->hshsiz > i) /* Locate into hash table directly. */
		return &i[(P_FREE_CHUNK *)((PUCHAR)ph + sizeof(HEAP_HEADER))];
	else /* Locate to biggest one. */
		return &(ph->hshsiz - 1)[(P_FREE_CHUNK *)((PUCHAR)ph + sizeof(HEAP_HEADER))];
}

void _nmUnlinkChunk(P_HEAP_HEADER ph, P_FREE_CHUNK ptr)
{
	register P_FREE_CHUNK pfc = ptr, pofc = pfc;
	register P_FREE_CHUNK * ppfc;

	if ((_nmCLZ(HEAD_NOTE(pfc) & ~(size_t)MASK)) - _nmCLZ(ph->size) <= ph->hshsiz)
	{
		ppfc = _nmLocateHashTable(ph, _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size));

		if (NULL != *ppfc)
		{
			pfc = *ppfc;
			do
			{
				pfc = pfc->p[FCP_PREV];
				if (ptr == pfc)
				{
					register size_t i;

					pfc->p[FCP_PREV]->p[FCP_NEXT] = pfc->p[FCP_NEXT];
					pfc->p[FCP_NEXT]->p[FCP_PREV] = pfc->p[FCP_PREV];

					i = _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size);
					if (pfc == *_nmLocateHashTable(ph, i)) /* Reach at linked list header.. */
						*_nmLocateHashTable(ph, i) = NULL;

					break;
				}
			} while (pfc != pofc);
		}
	}
}

void _nmRestoreEntrance(P_FREE_CHUNK * ppfc, P_FREE_CHUNK pfc)
{
	pfc->p[FCP_PREV] = pfc->p[FCP_NEXT] = pfc;
	if (NULL != *ppfc)
	{
		pfc->p[FCP_NEXT] = *ppfc;
		(*ppfc)->p[FCP_PREV]->p[FCP_NEXT] = pfc;
		pfc->p[FCP_PREV] = (*ppfc)->p[FCP_PREV]->p[FCP_PREV];
		(*ppfc)->p[FCP_PREV]->p[FCP_PREV] = pfc;
	}
	*ppfc = pfc;
}

void * _nmSplitChunk(P_HEAP_HEADER ph, P_FREE_CHUNK pfc, size_t size)
{
	register void * pt;
	register size_t i;

	i = HEAD_NOTE(pfc) - size - sizeof(size_t);
	i &= ~(size_t)MASK;
	HEAD_NOTE(pfc) = size;
	FOOT_NOTE(pfc) = size;
	pt = pfc;
	pfc = (P_FREE_CHUNK)((PUCHAR)pfc + size + sizeof(size_t) * 2);
	HEAD_NOTE(pfc) = i;
	FOOT_NOTE(pfc) = i;
	HEAD_NOTE(pfc) |= FREE_MASK;
	FOOT_NOTE(pfc) |= FREE_MASK;

	/* Search hash table and put new free chunk to the linked list. */
	_nmRestoreEntrance(_nmLocateHashTable(ph, _nmCLZ(i) - _nmCLZ(ph->size)), pfc);

	return pt;
}

void _nmPutChunk(P_HEAP_HEADER ph, P_FREE_CHUNK pfc)
{
	if ((_nmCLZ(HEAD_NOTE(pfc) & ~(size_t)MASK)) - _nmCLZ(ph->size) > ph->hshsiz)
	{
		HEAD_NOTE(pfc) |= FREE_MASK;
		FOOT_NOTE(pfc) |= FREE_MASK;
	}
	else
		_nmRestoreEntrance(_nmLocateHashTable(ph, _nmCLZ(HEAD_NOTE(pfc)) - _nmCLZ(ph->size)), pfc);
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

	hh.size = size - (sizeof(HEAP_HEADER) + (hshsiz * sizeof(P_FREE_CHUNK)));
	hh.size &= ~(size_t)MASK;
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
	HEAD_NOTE(pfc) = t;
	FOOT_NOTE(pfc) = t;
	HEAD_NOTE(pfc) |= FREE_MASK;
	FOOT_NOTE(pfc) |= FREE_MASK;

	/* Set free chunk structure. */
	fc.p[FCP_PREV] = fc.p[FCP_NEXT] = pfc;
	memcpy(pfc, &fc, sizeof(FREE_CHUNK));

	/* Set hash table. */
	*_nmLocateHashTable(pbase, _nmCLZ(t) - _nmCLZ(hh.size)) = pfc;

	return (P_HEAP_HEADER)pbase;
}

P_HEAP_HEADER nmExtendHeap(P_HEAP_HEADER ph, size_t sizincl)
{
	if (sizincl < MIN_CHUNK_SIZE)
		return NULL;
	else
	{
		/* Get the last chunk. */
		bool bused;
		size_t i;
		PUCHAR phead;
		P_FREE_CHUNK pfc;

		phead = (PUCHAR)ph + HEAP_BEGIN(ph);
		phead += ph->size - sizeof(size_t);

		i = *(size_t *)phead;
		bused = !!(i & ~(size_t)MASK);

		if (FREE != bused)
		{
			pfc = (P_FREE_CHUNK)(phead + sizeof(size_t) * 2);

			sizincl &= ~(size_t)MASK;
			ph->size += sizincl;
			sizincl -= sizeof(size_t) * 2;

			HEAD_NOTE(pfc) = sizincl;
			FOOT_NOTE(pfc) = sizincl;
			HEAD_NOTE(pfc) |= FREE_MASK;
			FOOT_NOTE(pfc) |= FREE_MASK;

			_nmPutChunk(ph, pfc);
		}
		else
		{
			phead -= (i & ~(size_t)MASK);
			pfc = (P_FREE_CHUNK)phead;

			sizincl += i;
			sizincl &= ~(size_t)MASK;

			_nmUnlinkChunk(ph, pfc);

			HEAD_NOTE(pfc) = sizincl;
			FOOT_NOTE(pfc) = sizincl;
			HEAD_NOTE(pfc) |= FREE_MASK;
			FOOT_NOTE(pfc) |= FREE_MASK;

			_nmPutChunk(ph, pfc);
		}

		return ph;
	}
}

void * nmAllocHeap(P_HEAP_HEADER ph, size_t size)
{
	register size_t i, j, k;
	register P_FREE_CHUNK pfc, * ppfc;

	size = ASIZE(size);
	j = _nmCLZ(size) - _nmCLZ(ph->size);

	/* Definitely cannot allocate. */
	if (j > _nmCLZ(size))
		return NULL;

	/* Search hash table. */
	ppfc = _nmLocateHashTable(ph, j);
	pfc = *ppfc;

	k = ppfc - (P_FREE_CHUNK *)((PUCHAR)ph + sizeof(HEAP_HEADER));

	for (i = 0; i < k; ++i)
	{
		if (NULL != pfc)
			break;
		--ppfc;
		pfc = *ppfc;
	}
	
	/* Search for fit chunk. */
	if (NULL != pfc)
	{
		register P_FREE_CHUNK pofc = pfc;
		do
		{
			if (size > (HEAD_NOTE(pfc) & ~(size_t)MASK))
				pfc = pfc->p[FCP_NEXT];
			else
				break;
		} while (pfc != pofc);
	}
	else
		return NULL; /* No available space. */

	if (size > (HEAD_NOTE(pfc) & ~(size_t)MASK))
		return NULL; /* Cannot find a suitable chunk. */
	else /* Cut one chunk. */
	{
		if (size == (HEAD_NOTE(pfc) & ~(size_t)MASK) || size > (HEAD_NOTE(pfc) & ~(size_t)MASK) - MIN_CHUNK_SIZE)
		{	/* No need to split. */
			_nmUnlinkChunk(ph, pfc);

			/* Set used. */
			HEAD_NOTE(pfc) &= USED_MASK;
			FOOT_NOTE(pfc) &= USED_MASK;

			return pfc;
		}
		else
		{
			_nmUnlinkChunk(ph, pfc);
			return _nmSplitChunk(ph, pfc, size); /* Split. */
		}
	}
}

void nmFreeHeap(P_HEAP_HEADER ph, void * ptr)
{
	register P_FREE_CHUNK pfc = (P_FREE_CHUNK)ptr;
	register bool bbtm = FREE, bhed = FREE;
	register size_t upcolsiz, upcolcnt;
	register size_t lwcolsiz, lwcolcnt;
	register size_t chksiz;
	register size_t i;

	/* Get upper bound. */
	if ((PUCHAR)pfc - sizeof(size_t) == (PUCHAR)ph + HEAP_BEGIN(ph))
		bhed = USED;

	upcolsiz = 0;
	upcolcnt = 0;

	do
	{
		if (FREE == bhed)
		{
			register size_t t = *(size_t *)((PUCHAR)pfc - 2 * sizeof(size_t));
			chksiz = (t & ~(size_t)MASK);
			if (FREE == !!(t & (size_t)MASK))
			{
				pfc = (P_FREE_CHUNK)((PUCHAR)pfc - 2 * sizeof(size_t) - chksiz);
				upcolsiz += chksiz;
				++upcolcnt;

				/* Unlink chunk. */
				_nmUnlinkChunk(ph, pfc);
			}
			else
				break;
		}
		else
			break;

		if ((PUCHAR)pfc - sizeof(size_t) == (PUCHAR)ph + HEAP_BEGIN(ph))
			bhed = USED;
	} while (USED != bhed);

	pfc = (P_FREE_CHUNK)ptr;

	/* Get lower bound. */
	chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;

	if ((PUCHAR)pfc + chksiz + sizeof(size_t) - ((PUCHAR)ph + HEAP_BEGIN(ph)) == ph->size)
		bbtm = USED;

	lwcolsiz = 0;
	lwcolcnt = 0;

	if (FREE == bbtm)
	{
		while (FREE == !!(*(size_t *)((PUCHAR)pfc + chksiz + sizeof(size_t)) & (size_t)MASK))
		{
			pfc = (P_FREE_CHUNK)((PUCHAR)pfc + chksiz + 2 * sizeof(size_t));
			chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;
			lwcolsiz += chksiz;
			++lwcolcnt;

			/* Unlink chunk. */
			_nmUnlinkChunk(ph, pfc);
		}
	}

	/* Coalesce up and downward. */
	pfc = (P_FREE_CHUNK)ptr;

	chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;

	i = upcolsiz + sizeof(size_t) * 2 * upcolcnt;

	if (upcolcnt)
		pfc = (P_FREE_CHUNK)((PUCHAR)pfc - i);

	chksiz += i;

	chksiz += lwcolsiz + sizeof(size_t) * 2 * lwcolcnt;

	HEAD_NOTE(pfc) = chksiz;
	FOOT_NOTE(pfc) = chksiz;
	HEAD_NOTE(pfc) |= FREE_MASK;
	FOOT_NOTE(pfc) |= FREE_MASK;

	/* Put free chunk back into hash table or leave it alone. */
	_nmPutChunk(ph, pfc);
}

void * nmReallocHeap(P_HEAP_HEADER ph, void * ptr, size_t size)
{
	register bool bbtm;
	register size_t j;
	register size_t chksiz;
	register size_t lwcolsiz, lwcolcnt;

	if (NULL == ptr)
		return nmAllocHeap(ph, size);

	size = ASIZE(size);
	j = _nmCLZ(size) - _nmCLZ(ph->size);

	/* Definitely cannot allocate. */
	if (j > _nmCLZ(size))
		return NULL;

	if (size < HEAD_NOTE((P_FREE_CHUNK)ptr))
		return _nmSplitChunk(ph, (P_FREE_CHUNK)ptr, size); /* Split. */
	else /* Try to collect residues. */
	{
		register P_FREE_CHUNK pfc;

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

		/* Coalesce downward. */
		pfc = (P_FREE_CHUNK)ptr;

		chksiz = HEAD_NOTE(pfc) & ~(size_t)MASK;

		chksiz += lwcolsiz + sizeof(size_t) * 2 * lwcolcnt;

		HEAD_NOTE(pfc) = chksiz;
		FOOT_NOTE(pfc) = chksiz;

		if (chksiz >= size)
		{
			HEAD_NOTE(pfc) &= USED_MASK;
			FOOT_NOTE(pfc) &= USED_MASK;
			return pfc;
		}
		else
		{
			register void * pr;
			pr = nmAllocHeap(ph, size);
			memcpy(pr, pfc, sizeof(HEAD_NOTE(pfc)));
			_nmPutChunk(ph, pfc);
			return pr;
		}
	}
}

#define T1 52
#define T2 60
#define T3 (T1 + 16)
#define T4 (25 * 4)

UCHART bbb[T4] = { 0 };

int main()
{
	memset(bbb, 0xff, T4);
	P_HEAP_HEADER p = nmCreateHeap(bbb, T4, 3);
	void * p2 = nmAllocHeap(p, 8);
	void * p3 = nmAllocHeap(p, 8);
	void * p4 = nmAllocHeap(p, 8);
	void * p5 = nmAllocHeap(p, 8);
	void * p6 = nmAllocHeap(p, 8);
	
	nmFreeHeap(p, p5);
	nmFreeHeap(p, p4);

	return 0;
}
