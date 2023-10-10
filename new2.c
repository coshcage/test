#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <wchar.h>
#include "StoneValley/src/svstring.h"
#include "StoneValley/src/svstack.h"
#include "StoneValley/src/svqueue.h"
#include "StoneValley/src/svtree.h"
#include "StoneValley/src/svset.h"

typedef enum en_Terminator
{
	T_LeftBracket = -1,
	T_Epsilon,
	T_Jumpover,
	T_Character,
	T_Selection,
	T_Concatenate,
	T_Closure,
	T_RightBracket,
	T_Error
} TERMINATOR;

typedef struct st_Lexicon
{
	TERMINATOR type;
	wchar_t    ch;
	BOOL       nullable;
	P_SET_T    firstpos;
	P_SET_T    lastpos;
} LEXICON, * P_LEXICON;

size_t gline   = 1;
size_t gcolumn = 1;

STACK_L stkOperand;
STACK_L stkOperator;

void Error(wchar_t msg[])
{
	wprintf(L"%s at line:%zu, column %zu.\n", msg, gline > 1 ? gline - 1 : gline, gcolumn - 1);
}

LEXICON Splitter(FILE * fp, BOOL * pbt)
{
	LEXICON l = { 0 };

	l.ch = fgetwc(fp);

	if (L'\\' == l.ch)
	{
		*pbt = !*pbt;
		if (FALSE == *pbt)
			l.type = T_Character;
		else
			l.type = T_Jumpover;
	}
	else
	{
		if (*pbt)
		{
			switch (l.ch)
			{
			case L'e':
				l.ch = L'\0';
				l.type = T_Epsilon;
				*pbt = FALSE;
				break;
			case L'n':
				l.ch = L'\n';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L't':
				l.ch = L'\t';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L'a':
				l.ch = L'\a';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L'r':
				l.ch = L'\r';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L'v':
				l.ch = L'\v';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L'f':
				l.ch = L'\f';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L'b':
				l.ch = L'\b';
				l.type = T_Character;
				*pbt = FALSE;
				break;
			case L'.':
			case L'|':
			case L'*':
			case L'(':
			case L')':
				l.type = T_Character;
				*pbt = FALSE;
				break;
			default:
				l.type = T_Character;
				*pbt = FALSE;
			}
		}
		else /* if FALSE == bt. */
		{
			switch (l.ch)
			{
			case L'.':
				l.type = T_Concatenate;
				break;
			case L'|':
				l.type = T_Selection;
				break;
			case L'*':
				l.type = T_Closure;
				break;
			case L'(':
				l.type = T_LeftBracket;
				break;
			case L')':
				l.type = T_RightBracket;
				break;
			default:
				l.type = T_Character;
			}
		}
	}
	return l;
}

void PrintLexicon(LEXICON lex)
{
	switch (lex.type)
	{
	case T_Character:
		if (WEOF == lex.ch)
			wprintf(L"CHAR: \'(#)\'\n");
		else if (L'\0' == lex.ch)
			wprintf(L"CHAR: \'\\e\'\n");
		else
			wprintf(L"CHAR: \'%c\'\n", lex.ch);
		break;
	case T_Selection:
		wprintf(L"|\n");
		break;
	case T_Closure:
		wprintf(L"*\n");
		break;
	case T_LeftBracket:
		wprintf(L"(\n");
		break;
	case T_RightBracket:
		wprintf(L")\n");
		break;
	case T_Concatenate:
		wprintf(L".\n");
		break;
	case T_Jumpover:
		wprintf(L"Jump\n");
		break;
	}
}

int cbftvsppp(void * pitem, size_t param)
{
	PrintLexicon(*(P_LEXICON)((P_TNODE_BY)pitem)->pdata);
	return CBF_CONTINUE;
}

void print2DUtil(P_TNODE_BY pnode, int space)
{
	// Base case
	if (NULL == pnode)
		return;

	// Increase distance between levels
	space += 10;

	// Process right child first
	print2DUtil(pnode->ppnode[RIGHT], space);

	// Print current node after space
	// count
	printf("\n");
	for (int i = 5; i < space; i++)
		printf(" ");
	//printf("%d\n", pnode->data);
	PrintLexicon(*(P_LEXICON)((P_TNODE_BY)pnode)->pdata);

	// Process left child
	print2DUtil(pnode->ppnode[LEFT], space);
}


P_TNODE_BY Parse(FILE * fp)
{
	BOOL bt = FALSE;
	LEXICON lex = { 0 }, tl = { 0 }, ttl = { 0 };
	P_TNODE_BY pnode, pnode1, pnode2, pnode3;
	size_t i = 1, j;

	stkInitL(&stkOperator);
	stkInitL(&stkOperand);

	do
	{
		i = 1;
		tl = lex;
		lex = Splitter(fp, &bt);

		if (T_Jumpover == lex.type)
			lex = tl;
		else
		{
			/* We need to insert concatenate in the following circumstances:
			 * a & b
			 * a & (
			 * ) & a
			 * ) & (
			 * * & a
			 * * & (
			 */
			switch (tl.type)
			{
			case T_Character:
				if (T_Character == lex.type || T_LeftBracket == lex.type)
					i = 2;
				break;
			case T_RightBracket:
				if (T_Character == lex.type || T_LeftBracket == lex.type)
					i = 2;
				break;
			case T_Closure:
				if (T_Character == lex.type || T_LeftBracket == lex.type)
					i = 2;
				break;
			}

			for (j = 0; j < i; ++j)
			{
				if (2 == i && 0 == j)
				{
					ttl = lex;
					lex.type = T_Concatenate;
				}
				else if (2 == i && 1 == j)
					lex = ttl;

				PrintLexicon(lex);
				switch (lex.type)
				{
				case T_Character:
					pnode = strCreateNodeD(&lex, sizeof(LEXICON));
					stkPushL(&stkOperand, &pnode, sizeof(P_TNODE_BY));
					break;
				case T_Selection:
				case T_Concatenate:
				case T_Closure:
					if (!stkIsEmptyL(&stkOperator))
					{
						stkPeepL(&pnode1, sizeof(P_TNODE_BY), &stkOperator);
						if (((P_LEXICON)pnode1->pdata)->type >= lex.type)
						{
							stkPopL(&pnode1, sizeof(P_TNODE_BY), &stkOperator);
							switch (((P_LEXICON)pnode1->pdata)->type)
							{
							case T_Selection:
							case T_Concatenate:
								stkPopL(&pnode2, sizeof(P_TNODE_BY), &stkOperand);
								stkPopL(&pnode3, sizeof(P_TNODE_BY), &stkOperand);

								pnode1->ppnode[RIGHT] = pnode2;
								pnode1->ppnode[LEFT] = pnode3;

								break;
							case T_Closure:
								stkPopL(&pnode2, sizeof(P_TNODE_BY), &stkOperand);
								pnode1->ppnode[LEFT] = pnode2;
								break;
							}
							stkPushL(&stkOperand, &pnode1, sizeof(P_TNODE_BY));
						}
					}
					pnode1 = strCreateNodeD(&lex, sizeof(LEXICON));
					stkPushL(&stkOperator, &pnode1, sizeof(P_TNODE_BY));
					break;
				case T_LeftBracket:
					pnode = strCreateNodeD(&lex, sizeof(LEXICON));
					stkPushL(&stkOperator, &pnode, sizeof(P_TNODE_BY));
					break;
				case T_RightBracket:
					for (;;)
					{
						if (!stkIsEmptyL(&stkOperator))
						{
							stkPeepL(&pnode1, sizeof(P_TNODE_BY), &stkOperator);
							if (T_LeftBracket != ((P_LEXICON)pnode1->pdata)->type)
							{
								stkPopL(&pnode1, sizeof(P_TNODE_BY), &stkOperator);
								switch (((P_LEXICON)pnode1->pdata)->type)
								{
								case T_Selection:
								case T_Concatenate:
									stkPopL(&pnode2, sizeof(P_TNODE_BY), &stkOperand);
									stkPopL(&pnode3, sizeof(P_TNODE_BY), &stkOperand);

									pnode1->ppnode[RIGHT] = pnode2;
									pnode1->ppnode[LEFT] = pnode3;

									break;
								case T_Closure:
									stkPopL(&pnode2, sizeof(P_TNODE_BY), &stkOperand);
									pnode1->ppnode[LEFT] = pnode2;
									break;
								}
								stkPushL(&stkOperand, &pnode1, sizeof(P_TNODE_BY));
							}
							else
								break;
						}
						else
							break;
					}
					stkPopL(&pnode1, sizeof(P_TNODE_BY), &stkOperator);
					strDeleteNodeD(pnode1);
					break;
				}
			}
		}
	}
	while (WEOF != lex.ch);

	for (;;)
	{
		if (!stkIsEmptyL(&stkOperator))
		{
			stkPopL(&pnode1, sizeof(P_TNODE_BY), &stkOperator);
			switch (((P_LEXICON)pnode1->pdata)->type)
			{
			case T_Selection:
			case T_Concatenate:
				stkPopL(&pnode2, sizeof(P_TNODE_BY), &stkOperand);
				stkPopL(&pnode3, sizeof(P_TNODE_BY), &stkOperand);

				pnode1->ppnode[RIGHT] = pnode2;
				pnode1->ppnode[LEFT] = pnode3;

				break;
			case T_Closure:
				stkPopL(&pnode2, sizeof(P_TNODE_BY), &stkOperand);
				pnode1->ppnode[LEFT] = pnode2;
				break;
			}
			stkPushL(&stkOperand, &pnode1, sizeof(P_TNODE_BY));
		}
		else
			break;
	}

	/* Return a syntax tree. */
	if (1 == strLevelLinkedListSC(stkOperand))
	{
		stkPeepL(&pnode, sizeof(P_TNODE_BY), &stkOperand);
		print2DUtil(pnode, 0);
	}
	else
	{
		printf("Error! Invalid regular expression.\n");
		while (!stkIsEmptyL(&stkOperand))
		{
			stkPopL(&pnode, sizeof(P_TNODE_BY), &stkOperand);
			treDeleteBY(pnode);
		}
		pnode = NULL;
	}

Lbl_ExitParser:
	stkFreeL(&stkOperand);
	stkFreeL(&stkOperator);
	return pnode;
}



int main(int argc, char ** argv)
{
	FILE * fp = fopen("C:\\Users\\user1\\source\\repos\\ConsoleApplication2\\ConsoleApplication2\\test.txt", "r");
	//FILE * fp = stdin;
	Parse(fp);
	fclose(fp);
	
	return 0;
}
