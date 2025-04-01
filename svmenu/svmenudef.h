#ifndef _SVMENUDEF_H_
#define _SVMENUDEF_H_

#include "svtree.h"

#define SMDF_DEFAULT           0
#define SMDF_BACK              1
#define SMDF_ON_SUB_MENU       2
#define SMDF_ON_EXIT           3
#define SMDF_ON_CHECK_TEST     4

typedef int (*SVM_MENU_ACTION)(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);

extern SVM_MENU_ACTION svm_fntbl[];

int smdfDefault(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfBack(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSubMenu(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnExit(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnCheckTest(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);

#endif

