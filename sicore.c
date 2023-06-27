/*
 * Name:        dict.c
 * Description: Offline dictionary.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0123230200Z0227230425L00219
 * License:     Public domain.
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*

*/
// SVIMRDB
#include "svimrdb.h"


static size_t _sizCUTTarget;

static int _sicbfcmp(const void * px, const void * py)
{
	int r = 0;
	size_t i;
	P_CELL pcx, pcy;

	pcx = **(P_CELL * *)px;
	pcy = **(P_CELL * *)py;

	for (i = 0; i < _sizCUTTarget; ++i)
	{
		switch (pcx->ct)
		{
		case CT_CHAR:
			r = *(char *)pcx->pdata - *(char *)pcy->pdata;
			break;
		case CT_SHORT:
			r = *(short *)pcx->pdata - *(short *)pcy->pdata;
			break;
		case CT_INTEGER:
			r = *(int *)pcx->pdata - *(int *)pcy->pdata;
			break;
		case CT_LONG:
			r = *(long *)pcx->pdata - *(long *)pcy->pdata;
			break;
		case CT_STRING:
			r = strcmp((char *)pcx->pdata, (char *)pcy->pdata);
			break;
		}
		if (0 == r)
			continue;
		else
			return r;
	}
	return r;
}


static int _sicbftvsMergeView(void * pitem, size_t param)
{
	memcpy
	(
		((P_MATRIX)(0[(size_t *)param]))->arrz.pdata + 1[(size_t *)param]++ * _sizCUTTarget * sizeof(P_CELL),
		*(P_CELL *)(P2P_BSTNODE(pitem)->knot.pdata),
		_sizCUTTarget * sizeof(P_CELL)
	);
	return CBF_CONTINUE;
}

P_MATRIX siCreateUniqueView(P_MATRIX pmtx)
{
	if (NULL == pmtx)
		return NULL;
	if (pmtx->ln && pmtx->col)
	{
		size_t a[2];
		P_MATRIX mtxr;
		SET_T set;
		size_t i, j;

		setInitT(&set);

		_sizCUTTarget = pmtx->col;

		for (j = i = 0; i < pmtx->ln; ++i)
		{
			P_CELL * ptuple = strGetValueMatrix(NULL, pmtx, i, 0, sizeof(P_CELL));

			if (!setIsMemberT(&set, &ptuple, _sicbfcmp))
			{
				set = treBSTInsertAA(set, &ptuple, sizeof(P_CELL *), _sicbfcmp);
				++j;
			}
		}

		mtxr = strCreateMatrix(j, pmtx->col, sizeof(P_CELL));
		a[0] = (size_t)mtxr;
		a[1] = 0;
		if (NULL != mtxr)
			setTraverseT(&set, _sicbftvsMergeView, (size_t)a, ETM_INORDER);

		setFreeT(&set);

		return mtxr;
	}
	return NULL;
}

P_MATRIX siCreateUnionView(P_MATRIX pmtxa, P_MATRIX pmtxb)
{
	P_CELL pc = NULL;
	
	P_MATRIX mtxr;
	size_t i, j;
	
	P_MATRIX pma = siCreateUniqueView(pmtxa);
	P_MATRIX pmb = siCreateUniqueView(pmtxb);

	if (NULL == pma && NULL == pmb)
	{
		return NULL;
	}
	else if (NULL != pma && NULL == pmb)
	{
		mtxr = strCreateMatrix(pma->ln, pma->col, sizeof(P_CELL));
		if (NULL != mtxr)
			strCopyArrayZ(&mtxr->arrz, &pma->arrz, sizeof(P_CELL));
	}
	else if (NULL == pma && NULL != pmb)
	{
		mtxr = strCreateMatrix(pmb->ln, pmb->col, sizeof(P_CELL));
		if (NULL != mtxr)
			strCopyArrayZ(&mtxr->arrz, &pmb->arrz, sizeof(P_CELL));
	}
	else
	{
		if (pma->col != pmb->col)
		{
			mtxr = strCreateMatrix(pma->ln + pmb->ln, pma->col > pmb->col ? pma->col : pmb->col, sizeof(P_CELL));

			strSetMatrix(mtxr, &pc, sizeof(P_CELL));

			for (i = 0; i < pma->ln; ++i)
			{
				memcpy
				(
					strGetValueMatrix(NULL, mtxr, i, 0, sizeof(P_CELL)),
					strGetValueMatrix(NULL, pma, i, 0, sizeof(P_CELL)),
					pma->col * sizeof(P_CELL)
				);
			}
			for (j = 0; j < pmb->ln; ++j, ++i)
			{
				memcpy
				(
					strGetValueMatrix(NULL, mtxr, i, 0, sizeof(P_CELL)),
					strGetValueMatrix(NULL, pmb, j, 0, sizeof(P_CELL)),
					pmb->col * sizeof(P_CELL)
				);
			}
		}
		else
		{
			SET_T set;
			size_t a[2];

			setInitT(&set);

			_sizCUTTarget = pma->col;

			for (j = i = 0; i < pma->ln; ++i)
			{
				P_CELL * ptuple = strGetValueMatrix(NULL, pma, i, 0, sizeof(P_CELL));

				if (!setIsMemberT(&set, &ptuple, _sicbfcmp))
				{
					set = treBSTInsertAA(set, &ptuple, sizeof(P_CELL *), _sicbfcmp);
					++j;
				}
			}

			for (i = 0; i < pmb->ln; ++i)
			{
				P_CELL * ptuple = strGetValueMatrix(NULL, pmb, i, 0, sizeof(P_CELL));

				if (!setIsMemberT(&set, &ptuple, _sicbfcmp))
				{
					set = treBSTInsertAA(set, &ptuple, sizeof(P_CELL *), _sicbfcmp);
					++j;
				}
			}

			mtxr = strCreateMatrix(j, pma->col, sizeof(P_CELL));
			a[0] = (size_t)mtxr;
			a[1] = 0;
			if (NULL != mtxr)
				setTraverseT(&set, _sicbftvsMergeView, (size_t)a, ETM_INORDER);

			setFreeT(&set);
		}
	}

	return mtxr;
}

P_MATRIX siCreateIntersectionView(P_MATRIX pmtxa, P_MATRIX pmtxb)
{
	if (NULL == pmtxa || NULL == pmtxb)
		return NULL;
	else
	{
		if (pmtxa->col != pmtxb->col)
			return NULL;
		else
		{
			P_MATRIX mtxr = NULL;
			size_t i;
			P_MATRIX pma = siCreateUniqueView(pmtxa);
			P_MATRIX pmb = siCreateUniqueView(pmtxb);
			SET_T seta, setb, * psetc;
			P_CELL * ptuple;
			size_t a[2];

			setInitT(&seta);
			setInitT(&setb);

			_sizCUTTarget = pma->col;

			for (i = 0; i < pma->ln; ++i)
			{
				ptuple = strGetValueMatrix(NULL, pma, i, 0, sizeof(P_CELL));
				seta = treBSTInsertAA(seta, &ptuple, sizeof(P_CELL *), _sicbfcmp);
			}

			for (i = 0; i < pma->ln; ++i)
			{
				ptuple = strGetValueMatrix(NULL, pmb, i, 0, sizeof(P_CELL));
				setb = treBSTInsertAA(setb, &ptuple, sizeof(P_CELL *), _sicbfcmp);
			}

			psetc = setCreateIntersectionT(&seta, &setb, sizeof(P_CELL), _sicbfcmp);
			if (NULL != psetc)
			{
				mtxr = strCreateMatrix(setSizeT(psetc), pma->col, sizeof(P_CELL));
				a[0] = (size_t)mtxr;
				a[1] = 0;
				if (NULL != mtxr)
					setTraverseT(psetc, _sicbftvsMergeView, (size_t)a, ETM_INORDER);
			}

			setFreeT(&seta);
			setFreeT(&setb);
			setDeleteT(psetc);

			return mtxr;
		}
	}
	return NULL;
}

P_MATRIX siCreateDifferenceView(P_MATRIX pmtxa, P_MATRIX pmtxb)
{
	P_MATRIX mtxr = NULL;
	P_MATRIX pma = siCreateUniqueView(pmtxa);
	P_MATRIX pmb = siCreateUniqueView(pmtxb);

	if (NULL == pmtxa)
		return NULL;
	else if (NULL == pmtxb)
	{
		mtxr = strCreateMatrix(pma->ln, pma->col, sizeof(P_CELL));
		if (NULL != mtxr)
			strCopyArrayZ(&mtxr->arrz, &pma->arrz, sizeof(P_CELL));
	}
	else
	{
		if (pma->col != pmb->col)
		{
			mtxr = strCreateMatrix(pma->ln, pma->col, sizeof(P_CELL));
			if (NULL != mtxr)
				strCopyArrayZ(&mtxr->arrz, &pma->arrz, sizeof(P_CELL));
		}
		else
		{
			size_t i;
			
			SET_T seta, setb, * psetc;
			P_CELL * ptuple;
			size_t a[2];

			setInitT(&seta);
			setInitT(&setb);

			_sizCUTTarget = pma->col;

			for (i = 0; i < pma->ln; ++i)
			{
				ptuple = strGetValueMatrix(NULL, pma, i, 0, sizeof(P_CELL));
				seta = treBSTInsertAA(seta, &ptuple, sizeof(P_CELL *), _sicbfcmp);
			}

			for (i = 0; i < pma->ln; ++i)
			{
				ptuple = strGetValueMatrix(NULL, pmb, i, 0, sizeof(P_CELL));
				setb = treBSTInsertAA(setb, &ptuple, sizeof(P_CELL *), _sicbfcmp);
			}

			psetc = setCreateDifferenceT(&seta, &setb, sizeof(P_CELL), _sicbfcmp);
			if (NULL != psetc)
			{
				mtxr = strCreateMatrix(setSizeT(psetc), pma->col, sizeof(P_CELL));
				a[0] = (size_t)mtxr;
				a[1] = 0;
				if (NULL != mtxr)
					setTraverseT(psetc, _sicbftvsMergeView, (size_t)a, ETM_INORDER);
			}

			setFreeT(&seta);
			setFreeT(&setb);
			setDeleteT(psetc);
		}
	}
	
	return mtxr;
}

P_MATRIX siCreateCartesianProductView(P_MATRIX pma, P_MATRIX pmb)
{
	size_t m, n;
	P_MATRIX pmr = strCreateMatrix(m = pma->ln * pmb->ln, n = pma->col + pma->col, sizeof(P_CELL));
	if (NULL != pmr)
	{
		REGISTER size_t i, x, y;
		for (i = 0, x = 0, y = 0; i < m; ++i)
		{
			/* Fill tuples of pma to pmr. */
			memcpy
			(
				strGetValueMatrix(NULL, pmr, i, 0, sizeof(P_CELL)),
				strGetValueMatrix(NULL, pma, x, 0, sizeof(P_CELL)),
				sizeof(P_CELL) * pma->col
			);
			/* Fill tuples of pmb to pmr. */
			memcpy
			(
				strGetValueMatrix(NULL, pmr, i, pma->col, sizeof(P_CELL)),
				strGetValueMatrix(NULL, pmb, y, 0, sizeof(P_CELL)),
				sizeof(P_CELL) * pmb->col
			);
			if (0 == (i + 1) % pmb->ln)
				++x;
			if (++y == pmb->ln)
				y = 0;
		}
	}
	return pmr;
}

P_MATRIX siCreateSelectView(P_MATRIX pmtx, SICBF_SELECT cbfsel, size_t param)
{
	P_MATRIX pmtxr = NULL;
	if (NULL == pmtx)
		return NULL;
	else
	{
		size_t i, j;
		pmtxr = strCreateMatrix(pmtx->ln, pmtx->col, sizeof(P_CELL));
		if (NULL != pmtxr)
		{
			for (j = i = 0; i < pmtx->ln; ++i)
			{
				if (FALSE != cbfsel(strGetValueMatrix(NULL, pmtx, i, 0, sizeof(P_CELL)), param))
				{
					memcpy
					(
						strGetValueMatrix(NULL, pmtxr, j, 0, sizeof(P_CELL)),
						strGetValueMatrix(NULL, pmtx, i, 0, sizeof(P_CELL)),
						sizeof(P_CELL) * pmtx->col
					);
					++j;
				}
			}
			if (NULL == strResizeMatrix(pmtxr, j, pmtx->col, sizeof(P_CELL)))
				return NULL;
			pmtxr->ln = j;
			pmtxr->col = pmtx->col;
		}
	}
	return pmtxr;
}

P_CELL siCreateCell(void * pitem, CellType ct)
{
	void * rtn;
	size_t l = sizeof(CELL), m = l;
	switch (ct)
	{
	case CT_CHAR:
		m = sizeof(char);
		l += m;
		break;
	case CT_SHORT:
		m = sizeof(short);
		l += m;
		break;
	case CT_INTEGER:
		m = sizeof(int);
		l += m;
		break;
	case CT_LONG:
		m = sizeof(long);
		l += m;
		break;
	case CT_STRING:
		m = strlen((char *)pitem) + 1;
		l += m;
		break;
	}
	rtn = malloc(l);
	if (NULL != rtn)
	{
		((P_CELL)rtn)->ct = ct;
		((P_CELL)rtn)->pdata = (PUCHAR)rtn + sizeof(CELL);
		memcpy(((P_CELL)rtn)->pdata, pitem, m);
	}
	return (P_CELL)rtn;
}

int cbftvs(void * pitem, size_t param)
{
	printf("%zu ", *(size_t *)pitem);
	//printf("%c ", (*(size_t *)pitem)[(char *)param]);
	return CBF_CONTINUE;
}

int main_test_unique(int argc, char * argv[])
{
	P_CELL pc;
	P_MATRIX pmtx, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtx = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtx, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtx, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtx, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtx, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtx, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtx, 2, 1, &pc, sizeof(P_CELL));
	
	/*
	strSetValueMatrix(pmtx, 0, 1, siCreateCell("a2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	*/

	pc = *(P_CELL *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(P_CELL));

	p = siCreateUniqueView(pmtx);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtx);
	strDeleteMatrix(p);

	return 0;
}

int main_union_test_0(int argc, char * argv[])
{
	P_CELL pc;
	P_MATRIX pmtxa, pmtxb, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtxa = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 1, &pc, sizeof(P_CELL));

	////////////////////////////////////////////////////////////////////////////////////////

	pmtxb = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("c1", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("c2", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("d1", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("d2", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 1, &pc, sizeof(P_CELL));

	/*
	strSetValueMatrix(pmtx, 0, 1, siCreateCell("a2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	*/

	// pc = *(P_CELL *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(P_CELL));

	p = siCreateUnionView(pmtxa, pmtxb);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtxa);
	strDeleteMatrix(pmtxb);
	strDeleteMatrix(p);

	return 0;
}

int main_test_union_1(int argc, char * argv[])
{
	P_CELL pc;
	P_MATRIX pmtxa, pmtxb, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtxa = strCreateMatrix(3, 3, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("a3", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 2, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b3", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 2, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b3", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 2, &pc, sizeof(P_CELL));

	////////////////////////////////////////////////////////////////////////////////////////
	pmtxb = NULL;
	/*
	pmtxb = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("c1", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("c2", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("d1", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("d2", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 1, &pc, sizeof(P_CELL));
	*/

	/*
	strSetValueMatrix(pmtx, 0, 1, siCreateCell("a2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	*/

	// pc = *(P_CELL *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(P_CELL));

	p = siCreateUnionView(pmtxa, pmtxb); // 5,3

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 2, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 2, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 1, sizeof(P_CELL)))->pdata);
	// printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 2, sizeof(P_CELL)))->pdata);
	printf("null\n");
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 2, sizeof(P_CELL)))->pdata);
	printf("null\n");
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 4, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 4, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 4, 2, sizeof(P_CELL)))->pdata);
	printf("null\n");

	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtxa);
	strDeleteMatrix(pmtxb);
	strDeleteMatrix(p);

	return 0;
}

int main_test_intersection_0(int argc, char * argv[])
{
	P_CELL pc;
	P_MATRIX pmtxa, pmtxb, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtxa = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 1, &pc, sizeof(P_CELL));

	////////////////////////////////////////////////////////////////////////////////////////

	pmtxb = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("c1", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("c2", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("d1", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("d2", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 1, &pc, sizeof(P_CELL));

	/*
	strSetValueMatrix(pmtx, 0, 1, siCreateCell("a2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	*/

	// pc = *(P_CELL *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(P_CELL));

	p = siCreateIntersectionView(pmtxa, pmtxb);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtxa);
	strDeleteMatrix(pmtxb);
	strDeleteMatrix(p);

	return 0;
}

int main_test_diff_0(int argc, char * argv[])
{
	P_CELL pc;
	P_MATRIX pmtxa, pmtxb, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtxa = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxa, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtxa, 2, 1, &pc, sizeof(P_CELL));

	////////////////////////////////////////////////////////////////////////////////////////

	pmtxb = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtxb, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("c1", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("c2", CT_STRING);
	strSetValueMatrix(pmtxb, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("d1", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("d2", CT_STRING);
	strSetValueMatrix(pmtxb, 2, 1, &pc, sizeof(P_CELL));

	/*
	strSetValueMatrix(pmtx, 0, 1, siCreateCell("a2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	*/

	// pc = *(P_CELL *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(P_CELL));

	p = siCreateDifferenceView(pmtxa, pmtxb);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 3, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtxa);
	strDeleteMatrix(pmtxb);
	strDeleteMatrix(p);

	return 0;
}

BOOL cbftestsel(P_CELL * pitem, size_t param)
{
	if (strcmp((*pitem)->pdata, "b1") == 0)
		return TRUE;
	return FALSE;
}

int main(int argc, char * argv[])
{
	P_CELL pc;
	P_MATRIX pmtx, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtx = strCreateMatrix(3, 2, sizeof(P_CELL));

	pc = siCreateCell("a1", CT_STRING);
	strSetValueMatrix(pmtx, 0, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("a2", CT_STRING);
	strSetValueMatrix(pmtx, 0, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtx, 1, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtx, 1, 1, &pc, sizeof(P_CELL));

	pc = siCreateCell("b1", CT_STRING);
	strSetValueMatrix(pmtx, 2, 0, &pc, sizeof(P_CELL));

	pc = siCreateCell("b2", CT_STRING);
	strSetValueMatrix(pmtx, 2, 1, &pc, sizeof(P_CELL));

	/*
	strSetValueMatrix(pmtx, 0, 1, siCreateCell("a2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 1, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 0, siCreateCell("b1", CT_STRING), sizeof(P_CELL));
	strSetValueMatrix(pmtx, 2, 1, siCreateCell("b2", CT_STRING), sizeof(P_CELL));
	*/

	// pc = *(P_CELL *)strGetValueMatrix(NULL, pmtx, 0, 0, sizeof(P_CELL));

	p = siCreateSelectView(pmtx, cbftestsel, 0);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 0, 1, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 0, sizeof(P_CELL)))->pdata);
	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);

	printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 2, 0, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)(*(P_CELL *)strGetValueMatrix(NULL, p, 1, 1, sizeof(P_CELL)))->pdata);
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	//printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtx);
	strDeleteMatrix(p);

	return 0;
}

int __b_main(int argc, char * argv[])
{
	P_MATRIX pmtx, p;
	char * pb1, * pb2;
	pb1 = "b1";
	pb2 = "b2";

	pmtx = strCreateMatrix(3, 2, sizeof(char *));

	strSetValueMatrix(pmtx, 0, 0, "a1", sizeof(char *));
	strSetValueMatrix(pmtx, 0, 1, "a2", sizeof(char *));
	strSetValueMatrix(pmtx, 1, 0, pb1, sizeof(char *));
	strSetValueMatrix(pmtx, 1, 1, pb2, sizeof(char *));
	strSetValueMatrix(pmtx, 2, 0, pb1, sizeof(char *));
	strSetValueMatrix(pmtx, 2, 1, pb2, sizeof(char *));

	p = siCreateUniqueView(pmtx);

	printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 0, sizeof(char *)));
	printf("%s\n", (char *)strGetValueMatrix(NULL, p, 0, 1, sizeof(char *)));
	printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 0, sizeof(char *)));
	printf("%s\n", (char *)strGetValueMatrix(NULL, p, 1, 1, sizeof(char *)));

	strDeleteMatrix(pmtx);
	strDeleteMatrix(p);

	return 0;
}

