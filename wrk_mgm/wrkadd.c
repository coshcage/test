#include <stdio.h>
#include "wrkmgm.h"

void AlterWorker(P_WORKER pw)
{
	int i, j, k;
	
	printf("Input worker's ID: ");
	scanf("%d", &pw->id);
	
	printf("Input worker's name: ");
	scanf("%s", pw->name);
	
	printf("Input worker's sex(1:Male/ 0:Female): ");
	scanf("%d", &i);
	pw->sex = i ? 'M' : 'F';
	
	printf("Input worker's department: ");
	scanf("%s", pw->dept);
	
	printf("Input worker's title: ");
	scanf("%s", pw->title);
	
	printf("Input worker's birthday(YYYY-MM-DD): ");
	scanf("%d-%d-%d", &i, &j, &k);
	sprintf(pw->birth, "%d-%d-%d", i, j, k);
	
	printf("Input worker's salary: ");
	scanf("%f", &pw->salary);
	
	printf("Input worker's telphone: ");
	scanf("%s", pw->tel);
}

void AddItem(void)
{
	WORKER w;
	P_NODE_D pnode;
	
	AlterWorker(&w);
	
	pnode = strCreateNodeD(&w, sizeof w);
					
	if (NULL == *g_pd)
	{
		*g_pd = pnode;
	}
	else
	{
		*g_pd = strInsertItemLinkedListDC(*g_pd, pnode, TRUE);
	}
}

