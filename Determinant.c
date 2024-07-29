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

MYTYPE Determinant(P_MATRIX pmtx)
{
	MYTYPE det = 0;
	size_t j;
	if (1 == pmtx->col)
	{
		return *(MYTYPE *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(int));
	}
	else
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
	MATRIX mtx;

	int a[4][4] = { 1,-2,3,-1,2,3,1,2,-3,2,0,3,7,1,1,1 };

	mtx.arrz.num = 9;
	mtx.arrz.pdata = (PUCHAR)a;
	mtx.ln = 3;
	mtx.col = 3;

	PrintMatrix(&mtx);

	printf("det(A)= %d\n", Determinant(&mtx));

	return 0;
}
