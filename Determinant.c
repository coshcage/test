#include <stdio.h>
#include "svstring.h"

#define SGN(_j) (((1 + (_j) + 1) & 1) ? -1 : 1)

typedef int MYTYPE;

void PrintMatrix(P_MATRIX pmtx)
{
	size_t i, j, size = sizeof(MYTYPE);
	for (i = 0; i < pmtx->ln; ++i)
	{
		for (j = 0; j < pmtx->col; ++j)
		{
			printf("%2d ", *(MYTYPE *)strGetValueMatrix(NULL, pmtx, i, j, size));
		}
		printf("\n");
	}
}

P_MATRIX CreateMinor(P_MATRIX pmtx, size_t mcol, size_t size)
{
	P_MATRIX pmtxr = strCreateMatrix(pmtx->ln - 1, pmtx->col - 1, size);
	if (NULL != pmtxr)
	{
		size_t i, j;
		for (i = 0; i < pmtxr->ln; ++i)
		{
			for (j = 0; j < pmtxr->col; ++j)
			{
				strSetValueMatrix(pmtxr, i, j, strGetValueMatrix(NULL, pmtx, i + 1, j < mcol ? j : j + 1, size), size);
			}
		}
	}
	return pmtxr;
}

MYTYPE Determinant2_2(P_MATRIX pmtx)
{
	if (2 == pmtx->ln)
	{
		return *(MYTYPE *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(MYTYPE)) *
			*(MYTYPE *)strGetValueMatrix(NULL, pmtx, 1, 1, sizeof(MYTYPE)) -
			*(MYTYPE *)strGetValueMatrix(NULL, pmtx, 0, 1, sizeof(MYTYPE)) *
			*(MYTYPE *)strGetValueMatrix(NULL, pmtx, 1, 0, sizeof(MYTYPE));
	}
	return 0;
}

MYTYPE Determinant(P_MATRIX pmtx)
{
	MYTYPE det = 0;
	size_t j;
	if (1 == pmtx->col)
	{
		return *(MYTYPE *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(MYTYPE));
	}
	else if (2 == pmtx->col)
	{
		return Determinant2_2(pmtx);
	}
	else if (pmtx->col == pmtx->ln)
	{
		for (j = 0; j < pmtx->col; ++j)
		{

			P_MATRIX pm = NULL;
			pm = CreateMinor(pmtx, j, sizeof(MYTYPE));
			det += (*(MYTYPE *)strGetValueMatrix(NULL, pmtx, 0, j, sizeof(MYTYPE)) * SGN(j) * Determinant(pm));
			strDeleteMatrix(pm);
		}
	}
	return det;
}

int main()
{
	size_t i, j;
	MATRIX mtx;

	int a[4][4] = { 1,-2,3,-1,2,3,1,2,-3,2,0,3,7,1,1,1 };

	mtx.arrz.num = 16;
	mtx.arrz.pdata = (PUCHAR)a;
	mtx.ln = 4;
	mtx.col = 4;

	PrintMatrix(&mtx);

	for (i = 0; i < 9999; ++i)
		j = Determinant(&mtx);

	return j;
}
