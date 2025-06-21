#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrkmgm.h"

void ReadFile(char * szfn)
{
	FILE * fp = fopen(szfn, "rb");
	if (fp)
	{
		while (!feof(fp))
		{
			P_NODE_D pnode;
			size_t rtn;
			WORKER w;
			rtn = fread(&w, sizeof(w), 1, fp);
			if (rtn)
			{	
				pnode = strCreateNodeD(&w, sizeof w);
					
				if (NULL == *g_pd)
				{
					*g_pd = pnode;
				}
				else
				{
					*g_pd = strInsertItemLinkedListDC(*g_pd, pnode, TRUE);
				}
			}
		}
		fclose(fp);
	}
}

int cbftvswf(void * pitem, size_t param)
{
	P_WORKER pw = (P_WORKER) ((P_NODE_D)pitem)->pdata;
	fwrite(pw, sizeof(WORKER), 1, (FILE *)param);
	return CBF_CONTINUE;
}

void WriteFile(char * szfn)
{
	FILE * fp = fopen(szfn, "wb");
	if (fp)
	{
		if (NULL != *g_pd)
			strTraverseLinkedListDC_N(*g_pd, NULL, cbftvswf, (size_t)fp, FALSE);
		fclose(fp);
	}
}
