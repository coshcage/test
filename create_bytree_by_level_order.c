#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "svtree.h"
#include "svqueue.h"

int _cbfCreateTree(void * pitem, size_t param)
{
	int c;
	char ch;
	ch = c = getchar();
	if (0 == c || EOF == c || '\n' == c)
		*(P_TNODE_BY **)pitem = NULL;
	else
		*(P_TNODE_BY **)pitem = strCreateNodeD(&ch, sizeof(char));
}

void CreateBYTreeByLevelOrder(P_BYTREE ptree)
{
	size_t i;
	QUEUE_L q;
	P_TNODE_BY pnode;

	if (NULL == *ptree)
	{
		*ptree = strCreateNodeD(&i, sizeof i);
	}
	queInitL(&q);
	if (NULL != *ptree)
	{
		queInsertL(&q, ptree, sizeof(P_TNODE_BY));

		while (!queIsEmptyL(&q))
		{
			queRemoveL(&pnode, sizeof(pnode), &q);
			if (NULL == pnode)
				continue;
			(void)scanf("%d", &i);
			*((size_t *)pnode->pdata) = i;
			if (0 == i)
			{
				pnode->ppnode[LEFT] = NULL;
				pnode->ppnode[RIGHT] = NULL;
			}
			else
			{
				pnode->ppnode[LEFT] = strCreateNodeD(&i, sizeof i);
				pnode->ppnode[RIGHT] = strCreateNodeD(&i, sizeof i);
				queInsertL(&q, &(pnode->ppnode[LEFT]), sizeof(P_TNODE_BY));
				queInsertL(&q, &(pnode->ppnode[RIGHT]), sizeof(P_TNODE_BY));
			}
		}

	}
	queFreeL(&q);
}

int cbftvs(void * pitem, size_t size)
{
	printf("%d ", *(size_t *)(((P_TNODE_BY)pitem)->pdata));
	return CBF_CONTINUE;
}

int main()
{
	P_BYTREE ptree;

	ptree = treCreateBY();

	CreateBYTreeByLevelOrder(ptree);

	treTraverseBYLevel(*ptree, cbftvs, 0);
	
	printf("\n");

	treDeleteBY(ptree);

	return 0;
}
