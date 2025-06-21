#ifndef _SVMENUDEF_H_
#define _SVMENUDEF_H_

#include "svtree.h"
#include "wrkmgm.h"

#define SMDF_DEFAULT           0
#define SMDF_BACK              1
#define SMDF_ON_SUB_MENU       2
#define SMDF_ON_EXIT           3
#define SMDF_ON_CHECK_TEST     4
#define SMDF_ON_ADD            5
#define SMDF_ON_VIEW           6
#define SMDF_ON_SRCH_ID        7
#define SMDF_ON_SRCH_NAME      8
#define SMDF_ON_SRCH_DEPT      9
#define SMDF_ON_SRCH_TITLE     10
#define SMDF_ON_ALTER          11
#define SMDF_DELETE_ID         12
#define SMDF_DELETE_NAME       13
#define SMDF_DELETE_DEPT       14
#define SMDF_SORT_ID           15
#define SMDF_SORT_SALARY       16
#define SMDF_SORT_BIRTH        17
#define SMDF_SORT_NAME         18
#define SMDF_STA_SALARY        19
#define SMDF_STA_BIRTH         20

typedef int (*SVM_MENU_ACTION)(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);

extern SVM_MENU_ACTION svm_fntbl[];

int smdfDefault(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfBack(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSubMenu(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnExit(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnCheckTest(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnAdd(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnView(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSearchByID(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSearchByName(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSearchByDept(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSearchByTitle(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnAlter(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnDeleteByID(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnDeleteByName(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnDeleteByDept(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSortByID(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSortBySalary(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSortByBirth(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnSortByName(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnStaSalary(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);
int smdfOnStaBirth(P_SVMENU pm, P_TNODE_G pgrandparent, P_TNODE_G parent, P_TNODE_G pself, P_TNODE_G * pout, void * param);

#endif

