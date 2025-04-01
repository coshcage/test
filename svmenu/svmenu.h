#ifndef _SVMENU_H_
#define _SVMENU_H_

#include "svtree.h"

#define SVM_CBF_EXIT CBF_TERMINATE

typedef P_GTREE P_SVMENU;
typedef GTREE   SVMENU;

typedef struct st_SVM_MENU_ITEM
{
	struct st_SVM_MENU_ITEM_MOD
	{
		UCHART bEnable  : 1;
		UCHART bChecked : 1;
		UCHART bVisible : 1;
	}
	mod;
	char * szCaption;
	size_t iAction;
} SVM_MENU_ITEM, * P_SVM_MENU_ITEM;

P_SVM_MENU_ITEM svmGetMenuItemPointer(P_TNODE_G pnode);
void svmSetMenuChecked(P_TNODE_G pnode, BOOL bChecked);
BOOL svmGetMenuChecked(P_TNODE_G pnode);
void svmSetMenuEnable(P_TNODE_G pnode, BOOL bEnable);
BOOL svmGetMenuEnable(P_TNODE_G pnode);
void svmSetMenuVisible(P_TNODE_G pnode, BOOL bVisible);
BOOL svmGetMenuVisible(P_TNODE_G pnode);
P_TNODE_G svmGetSubMenuItem(P_TNODE_G parent, size_t idx);
P_TNODE_G svmGetSubMenuItem(P_TNODE_G parent, size_t idx);
void svmShowMenuItem(P_TNODE_G pitem);
void svmCreateMenuItem(P_SVMENU pm, P_TNODE_G parent, char * name, size_t iact, size_t maxsub, ...);
void svmDeleteMenuItem(P_TNODE_G parent, P_TNODE_G pchild);
P_SVMENU svmCreateMenu(void);
void svmDeleteMenu(P_SVMENU pm);
int svmOnMenu(P_SVMENU pm, P_TNODE_G parent, size_t isub, P_TNODE_G * ppcur, void * param);
int svmDriver(P_SVMENU pm, void * param);

#endif

