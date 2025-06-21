#include <stdio.h>
#include <string.h>
#include "svdef.h"
#include "svmenu.h"
#include "svmenudef.h"
#include "wrkmgm.h"

SVM_MENU_ACTION svm_fntbl[] =
{
	smdfDefault,     /* 0. */
	smdfBack,        /* 1. */
	smdfOnSubMenu,   /* 2. */
	smdfOnExit,      /* 3. */
	smdfOnCheckTest, /* 4. */
	smdfOnAdd,       // 5
	smdfOnView,      // 6
	smdfOnSearchByID,   // 7
	smdfOnSearchByName, // 8
	smdfOnSearchByDept,  // 9
	smdfOnSearchByTitle, // 10
	smdfOnAlter,         // 11
	smdfOnDeleteByID,    // 12
	smdfOnDeleteByName,  // 13
	smdfOnDeleteByDept,  // 14
	smdfOnSortByID,      // 15
	smdfOnSortBySalary,  // 16
	smdfOnSortByBirth,   // 17
	smdfOnSortByName,    // 18
	smdfOnStaSalary,     // 19
	smdfOnStaBirth       // 20
};

int smdfDefault(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	return CBF_CONTINUE;
}

int smdfBack(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = pgrandparent;
	return CBF_CONTINUE;
}

int smdfOnSubMenu(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = pself;
	return CBF_CONTINUE;
}

int smdfOnExit(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	return CBF_TERMINATE;
}

int smdfOnCheckTest(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = parent;
	svmSetMenuChecked(pself, !svmGetMenuChecked(pself));
	return CBF_CONTINUE;
}

int smdfOnAdd(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	P_TNODE_G pnode;
	*pout = parent;
	AddItem();
	
	
	if (NULL == *g_pd)
	{
		pnode = svmGetSubMenuItem(*pm, 0);
		svmSetMenuVisible(pnode, FALSE);
		svmSetMenuEnable(pnode, FALSE);
		
		pnode = svmGetSubMenuItem(*pm, 2);
		svmSetMenuVisible(pnode, FALSE);
		svmSetMenuEnable(pnode, FALSE);
		
		pnode = svmGetSubMenuItem(*pm, 3);
		svmSetMenuVisible(pnode, FALSE);
		svmSetMenuEnable(pnode, FALSE);
		
		pnode = svmGetSubMenuItem(*pm, 4);
		svmSetMenuVisible(pnode, FALSE);
		svmSetMenuEnable(pnode, FALSE);
		
		pnode = svmGetSubMenuItem(*pm, 5);
		svmSetMenuVisible(pnode, FALSE);
		svmSetMenuEnable(pnode, FALSE);
		
		pnode = svmGetSubMenuItem(*pm, 6);
		svmSetMenuVisible(pnode, FALSE);
		svmSetMenuEnable(pnode, FALSE);
	}
	else
	{
		pnode = svmGetSubMenuItem(*pm, 0);
		svmSetMenuVisible(pnode, TRUE);
		svmSetMenuEnable(pnode, TRUE);
		
		pnode = svmGetSubMenuItem(*pm, 2);
		svmSetMenuVisible(pnode, TRUE);
		svmSetMenuEnable(pnode, TRUE);
		
		pnode = svmGetSubMenuItem(*pm, 3);
		svmSetMenuVisible(pnode, TRUE);
		svmSetMenuEnable(pnode, TRUE);
		
		pnode = svmGetSubMenuItem(*pm, 4);
		svmSetMenuVisible(pnode, TRUE);
		svmSetMenuEnable(pnode, TRUE);
		
		pnode = svmGetSubMenuItem(*pm, 5);
		svmSetMenuVisible(pnode, TRUE);
		svmSetMenuEnable(pnode, TRUE);
		
		pnode = svmGetSubMenuItem(*pm, 6);
		svmSetMenuVisible(pnode, TRUE);
		svmSetMenuEnable(pnode, TRUE);
	}
	return CBF_CONTINUE;
}

int smdfOnView(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = parent;
	ViewTable();
	
	return CBF_CONTINUE;
}

int smdfOnSearchByID(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	int i;
	*pout = parent;
	printf("Input ID: ");
	scanf("%d", &i);
	SearchItem(TRUE, 0, i, NULL);
	
	return CBF_CONTINUE;
}

int smdfOnSearchByName(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	char sz[BUFSIZ];
	*pout = parent;
	printf("Input name: ");
	scanf("%s", sz);
	SearchItem(TRUE, 1, 0, sz);
	
	return CBF_CONTINUE;
}

int smdfOnSearchByDept(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	char sz[BUFSIZ];
	*pout = parent;
	printf("Input department: ");
	scanf("%s", sz);
	SearchItem(TRUE, 2, 0, sz);
	
	return CBF_CONTINUE;
}

int smdfOnSearchByTitle(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	char sz[BUFSIZ];
	*pout = parent;
	printf("Input title: ");
	scanf("%s", sz);
	SearchItem(TRUE, 3, 0, sz);
	
	return CBF_CONTINUE;
}

int smdfOnAlter(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	int i;
	size_t r;
	*pout = parent;
	printf("Input ID: ");
	scanf("%d", &i);
	r = SearchItem(TRUE, 0, i, NULL);
	
	if (r)
	{
		P_NODE_D pnode = (P_NODE_D)r;
		P_WORKER pw = (P_WORKER)pnode->pdata;
		
		printf("Are you sure to alter?(1:Yes/ 0:No) ");
		scanf("%d", &i);
		if (i)
			AlterWorker(pw);
	}
	else
	{
		printf("Can not find worker.\n");
	}
	
	return CBF_CONTINUE;
}

int smdfOnDeleteByID(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	int i;
	size_t r;
	*pout = parent;
	printf("Input ID: ");
	scanf("%d", &i);
	r = SearchItem(TRUE, 0, i, NULL);
	
	if (r)
	{
		P_NODE_D pnode = (P_NODE_D)r;
		P_WORKER pw = (P_WORKER)pnode->pdata;
		
		printf("Are you sure to delete?(1:Yes/ 0:No) ");
		scanf("%d", &i);
		if (i)
		{
			P_NODE_D t = strRemoveItemLinkedListDC((P_NODE_D)r);
			
			strDeleteNodeD(t);
			
			if (t == *g_pd)
				*g_pd = NULL;
		}
	}
	else
	{
		printf("Can not find worker.\n");
	}
	
	return CBF_CONTINUE;
}

int smdfOnDeleteByName(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	int i;
	size_t r;
	char sz[BUFSIZ];
	*pout = parent;
	printf("Input name: ");
	scanf("%s", sz);

	r = SearchItem(TRUE, 1, 0, sz);
	
	if (r)
	{
		P_NODE_D pnode = (P_NODE_D)r;
		P_WORKER pw = (P_WORKER)pnode->pdata;
		
		printf("Are you sure to delete?(1:Yes/ 0:No) ");
		scanf("%d", &i);
		if (i)
		{
			P_NODE_D t = strRemoveItemLinkedListDC((P_NODE_D)r);
			
			strDeleteNodeD(t);
			
			if (t == *g_pd)
				*g_pd = NULL;
		}
	}
	else
	{
		printf("Can not find worker.\n");
	}
	
	return CBF_CONTINUE;
}

int smdfOnDeleteByDept(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	int i;
	size_t r;
	char sz[BUFSIZ];
	*pout = parent;
	printf("Input department: ");
	scanf("%s", sz);

	r = SearchItem(TRUE, 2, 0, sz);
	
	if (r)
	{
		P_NODE_D pnode = (P_NODE_D)r;
		P_WORKER pw = (P_WORKER)pnode->pdata;
		
		printf("Are you sure to delete?(1:Yes/ 0:No) ");
		scanf("%d", &i);
		if (i)
		{
			P_NODE_D t = strRemoveItemLinkedListDC((P_NODE_D)r);
			
			strDeleteNodeD(t);
			
			if (t == *g_pd)
				*g_pd = NULL;
		}
	}
	else
	{
		printf("Can not find worker.\n");
	}
	
	return CBF_CONTINUE;
}

int cbfcmpid(const void * px, const void * py)
{
	P_WORKER pwx = (P_WORKER)px, pwy = (P_WORKER)py;
	return pwx->id - pwy->id;
}

int cbfcmpsalary(const void * px, const void * py)
{
	P_WORKER pwx = (P_WORKER)px, pwy = (P_WORKER)py;
	if (pwx->salary > pwy->salary) return 1;
	if (pwx->salary < pwy->salary) return -1;
	return 0;
}

int cbfcmpbirth(const void * px, const void * py)
{
	P_WORKER pwx = (P_WORKER)px, pwy = (P_WORKER)py;
	return strcmp(pwx->birth, pwy->birth);
}

int cbfcmpname(const void * px, const void * py)
{
	P_WORKER pwx = (P_WORKER)px, pwy = (P_WORKER)py;
	return strcmp(pwx->name, pwy->name);
}

int smdfOnSortByID(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = parent;
	*g_pd = (P_NODE_D)strMergeSortLinkedListSDC(*g_pd, FALSE, ENT_DOUBLE, cbfcmpid);
	ViewTable();
	
	return CBF_CONTINUE;
}

int smdfOnSortBySalary(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = parent;
	*g_pd = (P_NODE_D)strMergeSortLinkedListSDC(*g_pd, FALSE, ENT_DOUBLE, cbfcmpsalary);
	ViewTable();
	
	return CBF_CONTINUE;
}

int smdfOnSortByBirth(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = parent;
	*g_pd = (P_NODE_D)strMergeSortLinkedListSDC(*g_pd, FALSE, ENT_DOUBLE, cbfcmpbirth);
	ViewTable();
	
	return CBF_CONTINUE;
}

int smdfOnSortByName(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	*pout = parent;
	*g_pd = (P_NODE_D)strMergeSortLinkedListSDC(*g_pd, FALSE, ENT_DOUBLE, cbfcmpname);
	ViewTable();
	
	return CBF_CONTINUE;
}

int cbftvsmidsalary(void * pitem, size_t param)
{
	size_t * pa = (size_t *)param;
	P_WORKER pw = (P_WORKER) ((P_NODE_D)pitem)->pdata;
	
	if (pw->salary < *(float *)pa[0])
		++pa[1];
	if (pw->salary > *(float *)pa[0])
		++pa[2];
	if (pw->salary == *(float *)pa[0])
		++pa[3];
	
	return CBF_CONTINUE;
}

int smdfOnStaSalary(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	size_t a[4];
	float f;
	
	*pout = parent;
	
	printf("Input middle salary: ");
	scanf("%f", &f);
	
	a[0] = (size_t)&f;
	a[1] = 0;
	a[2] = 0;
	a[3] = 0;
	
	if (NULL != *g_pd)
		strTraverseLinkedListDC_N(*g_pd, NULL, cbftvsmidsalary, (size_t)a, FALSE);
		
	printf("Below: %zd.\n", a[1]);
	printf("Above: %zd.\n", a[2]);
	printf("Equal: %zd.\n", a[3]);
	
	return CBF_CONTINUE;
}

int cbftvsearbirth(void * pitem, size_t param)
{
	size_t * pa = (size_t *)param;
	P_WORKER pw = (P_WORKER) ((P_NODE_D)pitem)->pdata;
	
	if (strcmp(pw->birth, (char *)pa[0]) <= 0)
		++pa[1];
	
	return CBF_CONTINUE;
}

int smdfOnStaBirth(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	size_t a[2];
	char sz[BUFSIZ];
	
	*pout = parent;
	
	printf("Input birthday: ");
	scanf("%s", sz);
	
	a[0] = (size_t)sz;
	a[1] = 0;
	
	if (NULL != *g_pd)
		strTraverseLinkedListDC_N(*g_pd, NULL, cbftvsearbirth, (size_t)a, FALSE);
		
	printf("People whose birthday early than it: %zd.\n", a[1]);
	
	return CBF_CONTINUE;
}

