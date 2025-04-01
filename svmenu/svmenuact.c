#include <stdio.h>

#include "svdef.h"
#include "svmenu.h"
#include "svmenudef.h"

SVM_MENU_ACTION svm_fntbl[] =
{
	smdfDefault,    /* 0. */
	smdfBack,       /* 1. */
	smdfOnSubMenu,  /* 2. */
	smdfOnExit,     /* 3. */
	smdfOnCheckTest /* 4. */
	
};

int smdfDefault(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param)
{
	/* TODO. */
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

