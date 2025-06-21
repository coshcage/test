#include <stdio.h>
#include "svmenu.h"
#include "svmenudef.h"
#include "wrkmgm.h"

#define SZ_DATA_FILE "data.dat"

P_LIST_D g_pd = NULL;

int main()
{
	P_TNODE_G pnode;
	P_SVMENU pm = svmCreateMenu();
	
	g_pd = strCreateLinkedListDC();
	
	svmCreateMenuItem
	(pm, NULL, "Main menu", SMDF_DEFAULT, 8,
		"View", SMDF_ON_VIEW,
		"Add", SMDF_ON_ADD,
		"Search", SMDF_ON_SUB_MENU,
		"Alter", SMDF_ON_ALTER,
		"Delete", SMDF_ON_SUB_MENU,
		"Sort", SMDF_ON_SUB_MENU,
		"Statistic", SMDF_ON_SUB_MENU,
		"Exit", SMDF_ON_EXIT
	);
	
	pnode = svmGetSubMenuItem(*pm, 2);
	
	svmCreateMenuItem
	(pm, pnode, NULL, SMDF_DEFAULT, 5,
		"Search by ID", SMDF_ON_SRCH_ID,
		"Search by name", SMDF_ON_SRCH_NAME,
		"Search by department", SMDF_ON_SRCH_DEPT,
		"Search by title", SMDF_ON_SRCH_TITLE,
		"Back", SMDF_BACK
	);
	
	pnode = svmGetSubMenuItem(*pm, 4);
	
	svmCreateMenuItem
	(pm, pnode, NULL, SMDF_DEFAULT, 4,
		"Delete by ID", SMDF_DELETE_ID,
		"Delete by name", SMDF_DELETE_NAME,
		"Delete by department", SMDF_DELETE_DEPT,
		"Back", SMDF_BACK
	);
	
	pnode = svmGetSubMenuItem(*pm, 5);
	
	svmCreateMenuItem
	(pm, pnode, NULL, SMDF_DEFAULT, 5,
		"Sort by ID", SMDF_SORT_ID,
		"Sort by salary", SMDF_SORT_SALARY,
		"Sort by date of birth", SMDF_SORT_BIRTH,
		"Sort by name", SMDF_SORT_NAME,
		"Back", SMDF_BACK
	);
	
	pnode = svmGetSubMenuItem(*pm, 6);
	
	svmCreateMenuItem
	(pm, pnode, NULL, SMDF_DEFAULT, 3,
		"Statistic by salary", SMDF_STA_SALARY,
		"Statistic by date of birth", SMDF_STA_BIRTH,
		"Back", SMDF_BACK
	);
	
	ReadFile(SZ_DATA_FILE);
	
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
	
	svmDriver(pm, NULL);
	
	WriteFile(SZ_DATA_FILE);
	
	strDeleteLinkedListDC(g_pd, FALSE);
	
	svmDeleteMenu(pm);
}
