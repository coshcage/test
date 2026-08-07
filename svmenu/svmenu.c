#include <stdio.h>
#include <stdarg.h>
#include "svmenu.h"
#include "svmenudef.h"

P_SVM_MENU_ITEM svmGetMenuItemPointer(P_TNODE_G pnode)
{
	return (P_SVM_MENU_ITEM)pnode->pdata;
}

void svmSetMenuChecked(P_TNODE_G pnode, bool bChecked)
{
	svmGetMenuItemPointer(pnode)->mod.bChecked = !!bChecked;
}

bool svmGetMenuChecked(P_TNODE_G pnode)
{
	return svmGetMenuItemPointer(pnode)->mod.bChecked;
}

void svmSetMenuEnable(P_TNODE_G pnode, bool bEnable)
{
	svmGetMenuItemPointer(pnode)->mod.bEnable = !!bEnable;
}

bool svmGetMenuEnable(P_TNODE_G pnode)
{
	return svmGetMenuItemPointer(pnode)->mod.bEnable;
}

void svmSetMenuVisible(P_TNODE_G pnode, bool bVisible)
{
	svmGetMenuItemPointer(pnode)->mod.bVisible = !!bVisible;
}

bool svmGetMenuVisible(P_TNODE_G pnode)
{
	return svmGetMenuItemPointer(pnode)->mod.bVisible;
}

P_TNODE_G svmGetSubMenuItem(P_TNODE_G parent, size_t idx)
{
	return *(P_TNODE_G *)strLocateItemArrayZ(&parent->children, sizeof(P_TNODE_G), idx);
}

void svmShowMenuItem(P_TNODE_G pitem)
{
	size_t i;
	puts(((P_SVM_MENU_ITEM)pitem->pdata)->szCaption);
	for (i = 0; i < strLevelArrayZ(&pitem->children); ++i)
	{
		P_TNODE_G pnode = svmGetSubMenuItem(pitem, i);
		
		if (svmGetMenuItemPointer(pnode)->mod.bVisible)
			printf
			(
				"\t[%zd] %s%s%s%s\n",
				i,
				svmGetMenuItemPointer(pnode)->mod.bChecked ? "[CHECKED] " : "",
				svmGetMenuItemPointer(pnode)->mod.bEnable ? "" : "[DISABLED] ",
				svmGetMenuItemPointer(pnode)->szCaption,
				0 == strLevelArrayZ(&pnode->children) ? "" : "..."
			);
	}
}

void svmCreateMenuItem(P_SVMENU pm, P_TNODE_G parent, char * name, size_t iact, size_t maxsub, ...)
{
	SVM_MENU_ITEM smi;
	P_TNODE_G pnode = parent;
	size_t i;
	va_list ap;
	
	va_start(ap, maxsub);
	
	if (NULL == *pm)
	{
		smi.mod.bEnable  = true;
		smi.mod.bChecked = false;
		smi.mod.bVisible = true;
		
		smi.szCaption = name;
		smi.iAction   = iact;
	
		pnode = treInsertG(parent, &smi, sizeof(SVM_MENU_ITEM));
		*pm = pnode;
	}
	
	for (i = 0; i < maxsub; ++i)
	{
		
		smi.mod.bEnable  = true;
		smi.mod.bChecked = false;
		smi.mod.bVisible = true;
		
		smi.szCaption = va_arg(ap, char *);
		smi.iAction   = va_arg(ap, size_t);
		
		treInsertG(pnode, &smi, sizeof(SVM_MENU_ITEM));
	}
	
	va_end(ap);
}

void svmDeleteMenuItem(P_TNODE_G parent, P_TNODE_G pchild)
{
	treRemoveSubtreeG(parent, pchild, true);
}

P_SVMENU svmCreateMenu(void)
{
	return treCreateG();
}

void svmDeleteMenu(P_SVMENU pm)
{
	treDeleteG(pm);
}

int svmOnMenu(P_SVMENU pm, P_TNODE_G parent, size_t isub, P_TNODE_G * ppcur, void * param)
{
	if (isub >= strLevelArrayZ(&parent->children))
	{
		if (NULL != ppcur)
			*ppcur = parent;
		return CBF_CONTINUE;
	}
	else
	{
		P_TNODE_G p = parent;
		P_TNODE_G pnode = svmGetSubMenuItem(parent, isub);
		P_SVM_MENU_ITEM pit = svmGetMenuItemPointer(pnode);
		
		p = treGetParentNodeG(*pm, parent);
		if (NULL == p)
			p = parent;
		
		if (pit->mod.bEnable)
			return svm_fntbl[pit->iAction](pm, p, parent, pnode, ppcur, param);
		return CBF_CONTINUE;
	}
}

void svmDriver(P_SVMENU pm, void * param)
{
	size_t i;
	P_TNODE_G pnode = *pm;
	
	do
	{
		svmShowMenuItem(pnode);
		printf("?> ");
		scanf("%zu", &i);
	}
	while (SVM_CBF_EXIT != svmOnMenu(pm, pnode, i, &pnode, param));
}

