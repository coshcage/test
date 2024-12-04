#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StoneValley/src/svstring.h"
#include "StoneValley/src/svset.h"

int PrintSetOri(void * pitem, size_t param)
{
	DWC4100(param);

	printf("%c, ", *(char *)P2P_TNODE_BY(pitem)->pdata);
	return CBF_CONTINUE;
}

int PrintSetPS(void * pitem, size_t param)
{
	DWC4100(param);
	printf("{");
	treMorrisTraverseBYIn(**(P_SET_T *)P2P_TNODE_BY(pitem)->pdata, PrintSetOri, 0);
	printf("} ");
	return CBF_CONTINUE;
}

int DestroySetPS(void * pitem, size_t param)
{
	DWC4100(param);
	setDeleteT(*(P_SET_T *)P2P_TNODE_BY(pitem)->pdata);
	return CBF_CONTINUE;
}

int FillArray(void * pitem, size_t param)
{

	*((*(char **)param)++) = *(char *)P2P_TNODE_BY(pitem)->pdata;
	return CBF_CONTINUE;
}

int cbfcmpchar(const void * px, const void * py)
{
	return *(char *)px - *(char *)py;
}

int _grpCBFCompareInteger(const void * px, const void * py);

int main()
{
	P_ARRAY_Z parr1;
	P_SET_T sOri, sPS;
	size_t i;
	int n;
	char sz[BUFSIZ], * p, ** pp;

	sOri = setCreateT();
	sPS = setCreateT();

	scanf("%s", sz);
	for (n = strlen(sz) - 1; n >= 0; --n)
	{
		setInsertT(sOri, sz + n, sizeof(char), cbfcmpchar);
	}

	printf("Input set: {");
	treMorrisTraverseBYIn(*sOri, PrintSetOri, 0);
	printf("}\n");

	parr1 = strCreateArrayZ(setSizeT(sOri), sizeof(char));
	p = parr1->pdata;
	pp = &p;
	treMorrisTraverseBYIn(*sOri, FillArray, (size_t)pp);

	for (i = 1; i <= strLevelArrayZ(parr1); ++i)
	{
		P_SET_T sub;
		P_ARRAY_Z parr2 = strCreateArrayZ(i, sizeof(char));
		memcpy(parr2->pdata, parr1->pdata, i);
		do
		{
			size_t j;
			sub = setCreateT();
			for (j = 0; j < strLevelArrayZ(parr2); ++j)
			{
				printf("\t%c ", *(char *)strLocateItemArrayZ(parr2, sizeof(char), j));
				setInsertT(sub, strLocateItemArrayZ(parr2, sizeof(char), j), sizeof(char), cbfcmpchar);
			}
			printf("\n");
			setInsertT(sPS, &sub, sizeof(P_SET_T), _grpCBFCompareInteger);
		} while (strCombineNextArrayZ(parr2, parr1, sizeof(char), cbfcmpchar));
		
		strDeleteArrayZ(parr2);
	}
	
	printf("{{}, ");
	treMorrisTraverseBYIn(*sPS, PrintSetPS, 0);
	printf("}\n");

	setDeleteT(sOri);
	strDeleteArrayZ(parr1);

	treMorrisTraverseBYIn(*sPS, DestroySetPS, 0);
	setDeleteT(sPS);

	return 0;
}
