/* Standard library.
 */
#include "std.h"

UCHART volatile array[MEM_SIZ]; /* Create a linear address space in heap. */

const PUCHAR volatile pmem = (const PUCHAR volatile)array;

volatile P_BLOCK_HEADER phead = NULL; /* This is the header pointer of block chain. */

/* Invoke this function before you use any function of this package.
 */
void mpkInitMemory(void)
{
	phead = (P_BLOCK_HEADER)pmem;
	phead->pblock = pmem + sizeof(BLOCK_HEADER);
	phead->pnext = (P_BLOCK_HEADER)(pmem + MEM_SIZ - sizeof(BLOCK_HEADER));
	phead->size = 0;
	phead->pnext->pblock = pmem + MEM_SIZ;
	phead->pnext->pnext = NULL;
	phead->pnext->size = 0;
}

void * memcpy(void * dst, void * src, size_t size)
{
	char * sc1;
	char * sc2;
	sc1 = dst;
	sc2 = src;
	if (sc2 < sc1 && sc1 < sc2 + size)
		for (sc1 += size, sc2 += size; 0 < size; --size)
			*--sc1 = *--sc2;
	else
		for (; 0 < size; --size)
			*sc1++ = *sc2++;
	return dst;
}

void * memset(void * s, int c, size_t n)
{
	const UCHART uc = c;
	unsigned char * su;
	for (su = s; 0 < n; ++su, --n)
		*su = uc;
	return s;
}

void * malloc(size_t size)
{
	if (size > 0)
	{
		P_BLOCK_HEADER pnode, pnew;
		for (pnode = phead; NULL != pnode; pnode = pnode->pnext)
		{
			if (pnode->pnext != NULL)
			{	/* Test available space for the new block. */
				if (pnode->pnext->pblock - sizeof(BLOCK_HEADER) - pnode->pblock - pnode->size >=
					size + sizeof(BLOCK_HEADER))
				{
					pnew = (P_BLOCK_HEADER)(pnode->pblock + pnode->size);
					/* Initialize the new block header and assign new block. */
					pnew->pblock = pnode->pblock + pnode->size + sizeof(BLOCK_HEADER);
					pnew->size = size;
					pnew->pnext = pnode->pnext;
					pnode->pnext = pnew;
					return pnew->pblock;
				}
			}
			else
				break;
		}
	}
	return NULL;
}

void free(void * ptr)
{	/* Preserve the tail and header block structure. */
	if (pmem == ptr || pmem + MEM_SIZ == ptr)
		return;
	if (NULL != ptr)
	{
		P_BLOCK_HEADER pnode;
		for (pnode = phead; NULL != pnode; pnode = pnode->pnext)
		{
			if (NULL != pnode->pnext && ptr == pnode->pnext->pblock)
			{
				pnode->pnext = pnode->pnext->pnext;
				return;
			}
		}
	}
	return;
}

void * realloc(void * ptr, size_t size)
{	/* Preserve the tail and header block structure. */
	if (pmem == ptr || pmem + MEM_SIZ == ptr)
		return NULL;
	if (NULL == ptr && size > 0)
		return malloc(size);
	else if (size > 0)
	{
		P_BLOCK_HEADER pnode;
		for (pnode = phead; NULL != pnode; pnode = pnode->pnext)
		{
			if (NULL != pnode->pnext && ptr == pnode->pnext->pblock)
			{
				if (size == pnode->pnext->size)
					return ptr;
				else if (pnode->pnext->pnext->pblock - sizeof(BLOCK_HEADER) - pnode->pblock - pnode->size >=
					size + sizeof(BLOCK_HEADER))
				{
					pnode->pnext->size = size;
					return ptr;
				}
				else
				{
					void * newptr = malloc(size);
					if (NULL != newptr)
					{
						memcpy(newptr, ptr, size <= pnode->pnext->size ? size : pnode->pnext->size);
						free(ptr);
					}
					return newptr;
				}
			}
		}
	}
	return NULL;
}

void * calloc(size_t n, size_t size)
{
	void * rtn = malloc(size * n);
	memset(rtn, 0, size * n);
	return rtn;
}

int memcmp(const void * s1, const void * s2, size_t n)
{
	PUCHAR su1, su2;
	for (su1 = (PUCHAR)s1, su2 = (PUCHAR)s2; 0 < n; ++su1, ++su2, --n)
		if (*su1 != *su2)
			return *su1 < *su2 ? -1 : 1;
	return 0;
}

unsigned int _s = 1;

void srand(unsigned int seed)
{
	_s = seed;
}

int rand(void)
{
	_s = _s * 0x000343FD + 0x00269EC3;
	return (_s >> 0x10) & 0x7FFF;
}
