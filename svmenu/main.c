#include <stdio.h>
#include "svmenu.h"
#include "svmenudef.h"

int main()
{
	P_TNODE_G pnode;
	P_SVMENU pm = svmCreateMenu();
	
	svmCreateMenuItem(pm, NULL, "Main menu", SMDF_DEFAULT, 3, "Exit", SMDF_ON_EXIT, "Sub menu", SMDF_ON_SUB_MENU, "Check test", SMDF_ON_CHECK_TEST);
	
	pnode = svmGetSubMenuItem(*pm, 1);
	
	svmCreateMenuItem(pm, pnode, NULL, SMDF_DEFAULT, 1, "Back", SMDF_BACK);
	
	svmSetMenuChecked(pnode, TRUE);
	
	pnode = svmGetSubMenuItem(*pm, 2);
	
	// svmSetMenuVisible(pnode, FALSE);
	
	svmDriver(pm, NULL);
	
	svmDeleteMenu(pm);
}
