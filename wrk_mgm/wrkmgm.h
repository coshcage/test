#ifndef _WRKMGM_H_
#define _WRKMGM_H_

#include "svstring.h"

typedef struct st_Worker
{
	int id;
	char name[32];
	char sex;
	char dept[32];
	char title[32];
	char birth[32];
	float salary;
	char tel[32];
} WORKER, * P_WORKER;

extern P_LIST_D g_pd;

void AlterWorker(P_WORKER pw);
void ReadFile(char * szfn);
void AddItem(void);
void ViewTable(void);
void WriteFile(char * szfn);
int cbftvsprint(void * pitem, size_t param);
size_t SearchItem(BOOL bprint, int m, int id, char * name);

#endif

