#include <stdio.h>
#include "StoneValley/src/svgraph.h"
#include "StoneValley/src/svqueue.h"
#include "StoneValley/src/svset.h"

int _grpCBFCompareInteger(const void * px, const void * py);

struct IndegreeTable { size_t vid; BOOL visited;  size_t indegree; };

int cbftvsgarr(void * pitem, size_t param)
{
	printf("%c ", *(size_t *)pitem);
	return CBF_CONTINUE;
}

int cbftvsInit(void * pitem, size_t param)
{
	P_VERTEX_L pvtx = (P_VERTEX_L)pitem;

	struct IndegreeTable it;
	it.vid = pvtx->vid;
	it.visited = FALSE;
	it.indegree = grpIndegreeVertexL((P_GRAPH_L)1[(size_t *)param], pvtx->vid);

	setInsertT((P_SET_T)0[(size_t *)param], &it, sizeof(struct IndegreeTable), _grpCBFCompareInteger);
	
	return CBF_CONTINUE;
}

int cbftvs_topo_decrease(void * pitem, size_t param)
{
	P_EDGE pedg = (P_EDGE)pitem;
	P_SET_T pset = (P_SET_T)0[(size_t *)param];

	P_BSTNODE pnode = treBSTFindData_X(*pset, &pedg->vid, _grpCBFCompareInteger);

	if (NULL != pnode)
	{
		struct IndegreeTable * pit = (struct IndegreeTable *)pnode->knot.pdata;
		--pit->indegree;
	}
	return CBF_CONTINUE;
}

int cbftvs_topo_increase(void * pitem, size_t param)
{
	P_EDGE pedg = (P_EDGE)pitem;
	P_SET_T pset = (P_SET_T)0[(size_t *)param];

	P_BSTNODE pnode = treBSTFindData_X(*pset, &pedg->vid, _grpCBFCompareInteger);

	if (NULL != pnode)
	{
		struct IndegreeTable * pit = (struct IndegreeTable *)pnode->knot.pdata;
		++pit->indegree;
	}
	return CBF_CONTINUE;
}

void topo_puppet(P_GRAPH_L pgrp, P_SET_T pset, P_DEQUE_DL seq, size_t n);

int cbftvs_print_seq(void * pitem, size_t param)
{
	printf("%c ", *(size_t *)((P_NODE_D)pitem)->pdata);
	DWC4100(param);
	return CBF_CONTINUE;
}

int cbftvstopo(void * pitem, size_t param)
{
	P_VERTEX_L pvtx = (P_VERTEX_L)pitem;
	P_DEQUE_DL seq = (P_DEQUE_DL)1[(size_t *)param];
	P_SET_T pset = (P_SET_T)0[(size_t *)param];
	P_GRAPH_L pgrp = (P_GRAPH_L)2[(size_t *)param];
	BOOL * pbf = (BOOL *)3[(size_t *)param];
	size_t n = 4[(size_t *)param];

	P_BSTNODE pnode = treBSTFindData_X(*pset, &pvtx->vid, _grpCBFCompareInteger);

	if (NULL != pnode)
	{
		struct IndegreeTable * pit = (struct IndegreeTable *)pnode->knot.pdata;
		if (0 == pit->indegree && FALSE == pit->visited)
		{
			grpTraverseVertexEdgesL(pgrp, pit->vid, cbftvs_topo_decrease, param);
			queInjectDL(seq, &pit->vid, sizeof(size_t));
			pit->visited = TRUE;
			topo_puppet(pgrp, pset, seq, n);

			pit->visited = FALSE;
			queEjectDL(NULL, sizeof(size_t), seq);
			grpTraverseVertexEdgesL(pgrp, pit->vid, cbftvs_topo_increase, param);

			*pbf = TRUE;
		}
	}
	
	return CBF_CONTINUE;
}



void topo_puppet(P_GRAPH_L pgrp, P_SET_T pset, P_DEQUE_DL seq, size_t n)
{
	size_t a[5];
	
	BOOL bfound = FALSE;

	a[0] = pset;
	a[1] = seq;
	a[2] = pgrp;
	a[3] = &bfound;
	a[4] = n;

	grpTraverseVerticesL(pgrp, cbftvstopo, (size_t)a);

	if (FALSE == bfound)
	{
		strTraverseLinkedListDC_A(seq->pfirst, NULL, cbftvs_print_seq, 0, FALSE);
		puts("\n");
	}
}


void topo2(P_GRAPH_L pgrp)
{
	P_SET_T pset;
	P_DEQUE_DL seq = queCreateDL();

	size_t a[2];

	pset = setCreateT();

	a[0] = pset;
	a[1] = pgrp;
	grpTraverseVerticesL(pgrp, cbftvsInit, (size_t)a);

	topo_puppet(pgrp, pset, seq, grpVerticesCountL(pgrp));

	setDeleteT(pset);
	queDeleteDL(seq);
}


int main____()
{
	P_GRAPH_L p;
	P_ARRAY_Z parr;

	p = grpCreateL();


	grpInsertVertexL(p, '1');
	grpInsertVertexL(p, '2');
	grpInsertVertexL(p, '3');
	grpInsertVertexL(p, '4');
	grpInsertVertexL(p, '5');

	grpInsertEdgeL(p, '1', '2', 0);
	grpInsertEdgeL(p, '1', '3', 0);
	grpInsertEdgeL(p, '2', '4', 0);
	grpInsertEdgeL(p, '3', '4', 0);
	grpInsertEdgeL(p, '3', '5', 0);
	grpInsertEdgeL(p, '4', '5', 0);
	

	/*
	grpInsertVertexL(p, '0');
	grpInsertVertexL(p, '1');
	grpInsertVertexL(p, '2');
	grpInsertVertexL(p, '3');
	grpInsertVertexL(p, '4');
	grpInsertVertexL(p, '5');

	grpInsertEdgeL(p, '4', '0', 0);
	grpInsertEdgeL(p, '4', '1', 0);
	grpInsertEdgeL(p, '5', '2', 0);
	grpInsertEdgeL(p, '5', '0', 0);
	grpInsertEdgeL(p, '2', '3', 0);
	grpInsertEdgeL(p, '3', '1', 0);
	*/

	parr = grpTopologicalSortL(p);

	if (NULL != parr)
	{
		//strTraverseArrayZ(parr, sizeof(size_t), cbftvsgarr, 0, FALSE);
		
		topo2(p);
		puts("\n");

		strDeleteArrayZ(parr);
	}

	grpDeleteL(p);

	return 0;
}
