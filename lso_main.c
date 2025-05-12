#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "svstring.h"
#include "svset.h"

typedef enum en_OPR
{
	OPR_UNION,
	OPR_INTERSECTION,
	OPR_DEFFERENCE
} OPR;

typedef struct st_LNSET
{
	FILE   * fp;
	P_SET_T set;
} LNSET, * P_LNSET;

void PrintHelp()
{
	printf("lso (Line Set Operations)");
	printf("lso [-u|-i|-d] file ...");
}

int cbfcmpsz(const void * px, const void * py)
{
	return strcmp(*(char **)px, *(char **)py);
}

int cbftvsfreesz(void * pitem, size_t param)
{
	P_NODE_D pn = (P_NODE_D)pitem;
	free(*(char **)pn->pdata);
	
	return CBF_CONTINUE;
}

int cbftvsprintsz(void * pitem, size_t param)
{
	P_NODE_D pn = (P_NODE_D)pitem;
	printf("%s\n", *(char **)pn->pdata);
	
	return CBF_CONTINUE;
}

void ReadFiles(P_ARRAY_Z parrsets)
{
	size_t i;
	P_ARRAY_Z szbuf = strCreateArrayZ(BUFSIZ, sizeof(char));
	for (i = 0; i < strLevelArrayZ(parrsets); ++i)
	{
		char * psz;
		size_t j = 0;
		P_LNSET pl = (P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i);
		while (!feof(pl->fp))
		{
			char c = fgetc(pl->fp);
			switch (c)
			{
			case '\n':
			case '\r':
			case '\0':
				if (!j)
					break;
				/* End of string. */
				*(char *)strLocateItemArrayZ(szbuf, sizeof(char), j) = 0;
				
				if (pl->set)
				{
					psz = strdup(szbuf->pdata);
					setInsertT(pl->set, &psz, sizeof(char *), cbfcmpsz);
				}
				
				j = 0;
				break;
			default:
				*(char *)strLocateItemArrayZ(szbuf, sizeof(char), j) = c;
				++j;
				if (j >= strLevelArrayZ(szbuf))
				{
					strResizeBufferedArrayZ(szbuf, sizeof(char), +BUFSIZ);
				}
			}
		}
	}
	strDeleteArrayZ(szbuf);
}

P_SET_T Solve(OPR opr, P_ARRAY_Z parrsets)
{
	size_t i;
	P_SET_T r;
	P_SET_T pset = setCopyT(((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), 0))->set,
	 sizeof(char *));
	r = pset;
	for (i = 1; i < strLevelArrayZ(parrsets); ++i)
	{
		P_LNSET pl = (P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i);
		
		switch (opr)
		{
		case OPR_UNION:
			r = setCreateUnionT(pset, pl->set, sizeof(char *), cbfcmpsz);
			break;
		case OPR_INTERSECTION:
			r = setCreateIntersectionT(pset, pl->set, sizeof(char *), cbfcmpsz);
			break;
		case OPR_DEFFERENCE:
			r = setCreateDifferenceT(pset, pl->set, sizeof(char *), cbfcmpsz);
			break;
		}
		setDeleteT(pset);
		pset = r;
	}
	
	return r;
}

int main(int argc, char ** argv)
{
	OPR opr;
	
	if (argc <= 2)
	{
		printf("Error arguments, type -h to show help.\n");
		return 1;
	}
	else
	{
		if (0 == strcmp("-u", argv[1]))
		{
			opr = OPR_UNION;
		}
		else if (0 == strcmp("-i", argv[1]))
		{
			opr = OPR_INTERSECTION;
		}
		else if (0 == strcmp("-d", argv[1]))
		{
			opr = OPR_DEFFERENCE;
		}
		/* Line read. */
		{
			size_t i;
			P_SET_T psetr;
			P_ARRAY_Z parrsets = strCreateArrayZ(argc - 2, sizeof(LNSET));
			
			/* Open files. */
			for (i = 0; i < strLevelArrayZ(parrsets); ++i)
			{
				((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->fp = fopen(argv[i + 2] , "r");
				((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->set = 
				(((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->fp) ?
				setCreateT() :
				NULL;
			}
			
			/* Read files. */
			ReadFiles(parrsets);
			
			/* Calculate and print result. */
			psetr = Solve(opr, parrsets);
			setTraverseT(psetr, cbftvsprintsz, 0, ETM_INORDER);
			
			setDeleteT(psetr);
			
			/* Close files. */
			for (i = 0; i < strLevelArrayZ(parrsets); ++i)
			{
				if (((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->fp)
				{
					fclose(((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->fp);
					/* Free sz. */
					setTraverseT(((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->set, cbftvsfreesz, 0, ETM_LEVELORDER);
					setDeleteT(((P_LNSET)strLocateItemArrayZ(parrsets, sizeof(LNSET), i))->set);
				}
			}
			strDeleteArrayZ(parrsets);
		}
	}
	return 0;
}

