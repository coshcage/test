#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <wchar.h>
#include "StoneValley/src/svstring.h"
#include "StoneValley/src/svstack.h"
#include "StoneValley/src/svqueue.h"
#include "StoneValley/src/svtree.h"
#include "StoneValley/src/svset.h"

#define DEBUG 1

#define TREE_NODE_SPACE_COUNT 10

typedef enum en_Terminator
{
	T_LeftBracket = -1,
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

typedef struct st_FollowPos
{
	size_t i;
	P_SET_T followpos;
} FOLLOWPOS, * P_FOLLOWPOS;

STACK_L stkOperand;
STACK_L stkOperator;

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
				l.type = T_Character;
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

int cbftvsPrintSet(void * pitem, size_t param)
{
	wprintf(L"%ld, ", *(size_t *)(P2P_TNODE_BY(pitem)->pdata));
	return CBF_CONTINUE;
}

void PrintLexicon(LEXICON lex)
{
	P_SET_T p, q;
	p = lex.firstpos;
	q = lex.lastpos;
	switch (lex.type)
	{
	case T_Character:
		if (WEOF == lex.ch)
			wprintf(L"CHAR: \'(#)\'  ");
		else if (L'\0' == lex.ch)
			wprintf(L"CHAR: \'\\e\' ");
		else
			wprintf(L"CHAR: \'%c\' ", lex.ch);
		break;
	case T_Selection:
		wprintf(L"| ");
		break;
	case T_Closure:
		wprintf(L"* ");
		break;
	case T_LeftBracket:
		wprintf(L"( ");
		break;
	case T_RightBracket:
		wprintf(L") ");
		break;
	case T_Concatenate:
		wprintf(L". ");
		break;
	case T_Jumpover:
		wprintf(L"Jump ");
		break;
	}
	wprintf(L"nulabl:%s {", lex.nullable ? L"TRUE" : L"FALSE");
	setTraverseT(p, cbftvsPrintSet, 0, ETM_INORDER);
	wprintf(L"} {");
	setTraverseT(q, cbftvsPrintSet, 0, ETM_INORDER);
	wprintf(L"}\n");
}

void PrintSyntaxTree(P_TNODE_BY pnode, size_t space)
{
	size_t i;
	if (NULL == pnode)
		return;

	space += TREE_NODE_SPACE_COUNT;

	// Process right child first
	PrintSyntaxTree(pnode->ppnode[RIGHT], space);

	// Print current node after space
	// count
	printf("\n");
	for (i = TREE_NODE_SPACE_COUNT; i < space; ++i)
		printf(" ");
	
	PrintLexicon(*(P_LEXICON)((P_TNODE_BY)pnode)->pdata);
	PrintSyntaxTree(pnode->ppnode[LEFT], space);
}

extern int _grpCBFCompareInteger(const void * px, const void * py);

int cbftvsComputeNullableAndPos(void * pitem, size_t param)
{
	P_TNODE_BY pnode = P2P_TNODE_BY(pitem);

	if (NULL != pnode->ppnode[LEFT]) /* pnode is not a leaf node. */
	{
		switch (((P_LEXICON)pnode->pdata)->type)
		{
		case T_Selection:
			/* Nullable. */
			((P_LEXICON)pnode->pdata)->nullable =
				((P_LEXICON)pnode->ppnode[LEFT]->pdata)->nullable ||
				((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->nullable;

			/* Firstpos. */
			((P_LEXICON)pnode->pdata)->firstpos =
				setCreateUnionT
				(
					((P_LEXICON)pnode->ppnode[LEFT]->pdata)->firstpos,
					((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->firstpos,
					sizeof(size_t), _grpCBFCompareInteger
				);

			/* Lastpos. */
			((P_LEXICON)pnode->pdata)->lastpos =
				setCreateUnionT
				(
					((P_LEXICON)pnode->ppnode[LEFT]->pdata)->lastpos,
					((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->lastpos,
					sizeof(size_t), _grpCBFCompareInteger
				);
			break;
		case T_Concatenate:
			/* Nullable. */
			((P_LEXICON)pnode->pdata)->nullable =
				((P_LEXICON)pnode->ppnode[LEFT]->pdata)->nullable &&
				((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->nullable;

			/* Firstpos. */
			if (((P_LEXICON)pnode->ppnode[LEFT]->pdata)->nullable)
				((P_LEXICON)pnode->pdata)->firstpos =
				setCreateUnionT
				(
					((P_LEXICON)pnode->ppnode[LEFT]->pdata)->firstpos,
					((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->firstpos,
					sizeof(size_t), _grpCBFCompareInteger
				);
			else
				((P_LEXICON)pnode->pdata)->firstpos =
					NULL != ((P_LEXICON)pnode->ppnode[LEFT]->pdata)->firstpos ?
					setCopyT(((P_LEXICON)pnode->ppnode[LEFT]->pdata)->firstpos, sizeof(size_t)) :
					NULL;

			/* Lastpos. */
			if (((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->nullable)
				((P_LEXICON)pnode->pdata)->lastpos =
				setCreateUnionT
				(
					((P_LEXICON)pnode->ppnode[LEFT]->pdata)->lastpos,
					((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->lastpos,
					sizeof(size_t), _grpCBFCompareInteger
				);
			else
				((P_LEXICON)pnode->pdata)->lastpos =
				NULL != ((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->lastpos ?
				setCopyT(((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->lastpos, sizeof(size_t)) :
				NULL;
			break;
		case T_Closure:
			/* Nullable. */
			((P_LEXICON)pnode->pdata)->nullable = TRUE;

			/* Firstpos. */
			((P_LEXICON)pnode->pdata)->firstpos =
				NULL != ((P_LEXICON)pnode->ppnode[LEFT]->pdata)->firstpos ?
				setCopyT(((P_LEXICON)pnode->ppnode[LEFT]->pdata)->firstpos, sizeof(size_t)) :
				NULL;

			/* Lastpos. */
			((P_LEXICON)pnode->pdata)->lastpos =
				NULL != ((P_LEXICON)pnode->ppnode[LEFT]->pdata)->lastpos ?
				setCopyT(((P_LEXICON)pnode->ppnode[LEFT]->pdata)->lastpos, sizeof(size_t)) :
				NULL;
			break;
		}
	}
	return CBF_CONTINUE;
}

int cbftvsCleanStruct(void * pitem, size_t param)
{
	P_LEXICON plex = (P_LEXICON)(P2P_TNODE_BY(pitem)->pdata);

	if (NULL != plex->firstpos)
		setDeleteT(plex->firstpos);
	if (NULL != plex->lastpos)
		setDeleteT(plex->lastpos);

	return CBF_CONTINUE;
}

void DestroySyntaxTree(P_TNODE_BY pnode)
{
	treTraverseBYPost(pnode, cbftvsCleanStruct, 0);
	treFreeBY(&pnode);
}

P_TNODE_BY Parse(FILE * fp, size_t * pleaves)
{
	BOOL bt = FALSE;
	size_t posCtr = 1;
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
			/* We need to insert concatenation in the following circumstances:
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
					if (L'\0' != lex.ch)
					{
						lex.firstpos = setCreateT();
						setInsertT(lex.firstpos, &posCtr, sizeof(size_t), _grpCBFCompareInteger);
						lex.lastpos = setCreateT();
						setInsertT(lex.lastpos, &posCtr, sizeof(size_t), _grpCBFCompareInteger);
						++posCtr;
					}
					else
						lex.firstpos = NULL;

					lex.nullable = L'\0' == lex.ch ? TRUE : FALSE;

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
					lex.lastpos = lex.firstpos = NULL;
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
		stkPeepL(&pnode, sizeof(P_TNODE_BY), &stkOperand);
	else
	{
#if DEBUG
		printf("Error! Invalid regular expression.\n");
#endif
		while (!stkIsEmptyL(&stkOperand))
		{
			stkPopL(&pnode, sizeof(P_TNODE_BY), &stkOperand);
			DestroySyntaxTree(pnode);
		}
		*pleaves = posCtr - 1;
		pnode = NULL;
	}

	stkFreeL(&stkOperand);
	stkFreeL(&stkOperator);

	*pleaves = posCtr - 1;
	return pnode;
}

int cbftvsStarTravFirstpos(void * pitem, size_t param)
{
	P_ARRAY_Z parr = (P_ARRAY_Z)0[(size_t *)param];

	P_SET_T * ppset = (P_SET_T *)strLocateItemArrayZ(parr, sizeof(P_SET_T), 1[(size_t *)param] - 1);

	if (NULL == *ppset)
		*ppset = setCreateT();

	setInsertT(*ppset, (size_t *)P2P_TNODE_BY(pitem)->pdata, sizeof(size_t), _grpCBFCompareInteger);

	return CBF_CONTINUE;
}

int cbftvsStarTravLastpos(void * pitem, size_t param)
{
	size_t a[2];

	P_SET_T firstpos = (P_SET_T)1[(size_t *)param];

	a[0] = 0[(size_t *)param];
	a[1] = *(size_t *)P2P_TNODE_BY(pitem)->pdata;

	setTraverseT(firstpos, cbftvsStarTravFirstpos, (size_t)a, ETM_INORDER);
	return CBF_CONTINUE;
}

int cbftvsComputeFollowpos(void * pitem, size_t param)
{
	size_t a[2];
	P_TNODE_BY pnode = P2P_TNODE_BY(pitem);

	a[0] = param;

	if (T_Closure == ((P_LEXICON)pnode->pdata)->type)
	{
		a[1] = (size_t)((P_LEXICON)pnode->pdata)->firstpos;
		setTraverseT(((P_LEXICON)pnode->pdata)->lastpos, cbftvsStarTravLastpos, (size_t)a, ETM_INORDER);
	}
	else if (T_Concatenate == ((P_LEXICON)pnode->pdata)->type)
	{
		a[1] = (size_t)((P_LEXICON)pnode->ppnode[RIGHT]->pdata)->firstpos;
		setTraverseT(((P_LEXICON)pnode->ppnode[LEFT]->pdata)->lastpos, cbftvsStarTravLastpos, (size_t)a, ETM_INORDER);
	}

	return CBF_CONTINUE;
}

P_ARRAY_Z CreateFollowPosArray(P_TNODE_BY pnode, size_t inodes)
{
	P_SET_T nil = NULL;

	P_ARRAY_Z parr = strCreateArrayZ(inodes, sizeof(P_SET_T));

	strSetArrayZ(parr, &nil, sizeof(P_SET_T));

	treTraverseBYPost(pnode, cbftvsComputeFollowpos, (size_t)parr);

	return parr;
}

int cbftvsPrintFollowposArray(void * pitem, size_t param)
{
	P_SET_T pset = *(P_SET_T *)pitem;
	wprintf(L"%ld\t{", ++0[(size_t *)param]);
	setTraverseT(pset, cbftvsPrintSet, 0, ETM_INORDER);
	wprintf(L"}\n");
	return CBF_CONTINUE;
}

int cbftvsClearSetT(void * pitem, size_t param)
{
	P_SET_T pset = *(P_SET_T *)pitem;
	if (NULL != pset)
		setDeleteT(pset);
	return CBF_CONTINUE;
}

void DestroyFollowposArray(P_ARRAY_Z parr)
{
	strTraverseArrayZ(parr, sizeof(P_SET_T), cbftvsClearSetT, 0, FALSE);
	strDeleteArrayZ(parr);
}

int main(int argc, char ** argv)
{
	size_t i, j = 0;
	P_ARRAY_Z parrfollowpos;
	P_TNODE_BY pnode;
	FILE * fp = fopen("C:\\Users\\user1\\source\\repos\\ConsoleApplication2\\ConsoleApplication2\\test.txt", "r");
	//FILE * fp = stdin;
	pnode = Parse(fp, &i);

	treTraverseBYPost(pnode, cbftvsComputeNullableAndPos, 0);
	PrintSyntaxTree(pnode, 0);

	parrfollowpos = CreateFollowPosArray(pnode, i);
	strTraverseArrayZ(parrfollowpos, sizeof(P_SET_T), cbftvsPrintFollowposArray, (size_t)&j, FALSE);

	DestroySyntaxTree(pnode);
	DestroyFollowposArray(parrfollowpos);

	printf("\t%d\n", i);
	fclose(fp);
	
	return 0;
}
