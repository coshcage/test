#include <stdio.h>
#include "wrkmgm.h"

int cbftvsprint(void * pitem, size_t param)
{
	P_WORKER pw = (P_WORKER) ((P_NODE_D)pitem)->pdata;
	printf("%d\t%s\t%c\t%s\t%s\t%s\t%.2f\t%s\n", pw->id, pw->name, pw->sex, pw->dept, pw->title, pw->birth, pw->salary, pw->tel);
	return CBF_CONTINUE;
}

void ViewTable(void)
{
	if (NULL != *g_pd)
		strTraverseLinkedListDC_N(*g_pd, NULL, cbftvsprint, 0, FALSE);
}
