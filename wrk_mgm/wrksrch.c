#include <stdio.h>
#include <string.h>
#include "wrkmgm.h"

int cbftvsprintsel(void * pitem, size_t param)
{
	int i;
	void * tar = NULL;
	size_t * pa = (size_t *)param;
	P_WORKER pw = (P_WORKER) ((P_NODE_D)pitem)->pdata;
	
	i = (int)pa[1];
	
	switch (pa[0])
	{
	case 0: // Search by id;
		if (pw->id == i)
			tar = pitem;
		break;
	case 1:	// search by name.
		if (strcmp(pw->name, (char *)pa[2]) == 0)
			tar = pitem;
		break;
	case 2:	// search by dept.
		if (strcmp(pw->dept, (char *)pa[2]) == 0)
			tar = pitem;
		break;
	case 3:	// search by title.
		if (strcmp(pw->title, (char *)pa[2]) == 0)
			tar = pitem;
		break;
	}
	
	if (pa[3])
	{
		if (NULL != tar)
			cbftvsprint(tar, 0);
	}
	
	if (NULL != tar)
		pa[4] = (size_t)tar;
	
	return CBF_CONTINUE;
}

size_t SearchItem(BOOL bprint, int m, int id, char * name)
{
	size_t a[5];
	a[0] = (size_t)m;
	a[1] = (size_t)id;
	a[2] = (size_t)name;
	a[3] = bprint;
	a[4] = 0;
	if (NULL != *g_pd)
		strTraverseLinkedListDC_N(*g_pd, NULL, cbftvsprintsel, (size_t)a, FALSE);
	return a[4];
}

