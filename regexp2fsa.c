#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svgraph.h"
#include "svstack.h"
#include "svqueue.h"
 
STACK_L stkOperator, stkOperand;
QUEUE_L queRegexp;
GRAPH_L machine;
 
struct st_Operator
{
    char name;
    char level;
} operators[] = { '(', -1, ')', -1, '*', 2, '|', 1 };
 
typedef struct st_Element
{
    union un_Data
    {
        char operator;
        char operand;
    } data;
    BOOL isOperator;
} Element, * P_Element;
 
typedef struct st_Node
{
    size_t x;
    size_t y;
} Node, * P_Node;
 
/* Finding info for edges. */
typedef struct _st_FIEDG {
    EDGE     vertex;
    P_NODE_S pnode;
    size_t   bweight; /* TRUE for weighted graph; FALSE for unweighted graph. */
} _FIEDG, * _P_FIEDG;
 
int cbftvsFindMaxData(void * pitem, size_t param)
{
    if (*(size_t *)((P_TNODE_B)pitem)->pdata > * (size_t *)param)
        *(size_t *)param = *(size_t *)((P_TNODE_B)pitem)->pdata;
    return CBF_CONTINUE;
}
 
size_t GenerateUniqueState(P_GRAPH_L pgrp)
{
    if (setIsEmptyT(pgrp))
        return 1;
    {
        size_t r = 0;
        setTraverseT(pgrp, cbftvsFindMaxData, (size_t)&r, ETM_LEVELORDER);
        return r + 1;
    }
}
 
char GetOperator(char o)
{
    int i;
    for (i = 0; i < sizeof operators / sizeof operators[0]; ++i)
        if (operators[i].name == o)
            return operators[i].level;
    return 0;
}
 
int Splitter(char * pstr)
{
    BOOL turn = FALSE;
    char c;
    Element ele;
    while (*pstr != '\0')
    {
        switch (*pstr)
        {
        case '\\':
            if (turn)
            {
                ele.data.operand = *pstr;
                ele.isOperator = FALSE;
                queInsertL(&queRegexp, &ele, sizeof(Element));
                turn = FALSE;
                break;
            }
            turn = TRUE;
            break;
        case '(':
        case ')':
        case '|':
        case '*':
            if (turn)
            {
                ele.data.operand = *pstr;
                ele.isOperator = FALSE;
                queInsertL(&queRegexp, &ele, sizeof(Element));
            }
            else
            {
                switch (*pstr)
                {
                case '(':
                    break;
                case ')':
                    if (! stkIsEmptyL(&stkOperator))
                    {
                        stkPeepL(&c, sizeof(char), &stkOperator);
                        while (c != '(')
                        {
                            stkPopL(&c, sizeof(char), &stkOperator);
                            ele.data.operand = c;
                            ele.isOperator = TRUE;
                            queInsertL(&queRegexp, &ele, sizeof(Element));
                            if (stkIsEmptyL(&stkOperator))
                                break;
                            else
                                stkPeepL(&c, sizeof(char), &stkOperator);
                        }
                        stkPopL(&c, sizeof(char), &stkOperator); /* Drop right brace. */
                    }
                    else
                    {
                        printf("Right brace mismatch.\n\n");
                        return 1;
                    }
                    goto Lbl_PassOperator;
                default:
                    if (! stkIsEmptyL(&stkOperator))
                    {
                        stkPeepL(&c, sizeof(char), &stkOperator);
                        while (GetOperator(c) >= GetOperator(*pstr))
                        {
                            stkPopL(&c, sizeof(char), &stkOperator);
                            ele.data.operand = c;
                            ele.isOperator = TRUE;
                            queInsertL(&queRegexp, &ele, sizeof(Element));
                            if (stkIsEmptyL(&stkOperator))
                                break;
                            else
                                stkPeepL(&c, sizeof(char), &stkOperator);
                        }
                    }
                    break;
                }
                stkPushL(&stkOperator, pstr, sizeof(char));
            }
        Lbl_PassOperator:
            turn = FALSE;
            break;
        default:
            ele.data.operand = *pstr;
            ele.isOperator = FALSE;
            queInsertL(&queRegexp, &ele, sizeof(Element));
            turn = FALSE;
            break;
        }
        ++pstr;
    }
    while (! stkIsEmptyL(&stkOperator))
    {
        stkPopL(&c, sizeof(char), &stkOperator);
        if (c == '(')
        {
            puts("Left brace mismatch.\n");
            return 1;
        }
        ele.data.operand = c;
        ele.isOperator = TRUE;
        queInsertL(&queRegexp, &ele, sizeof(Element));
    }
    if (queIsEmptyL(&queRegexp))
    {
        puts("Invalid Expression.\n");
        return 1;
    }
    return 0;
}
 
/*
 *  _______          __a__
 *  |     |         |     V
 * (A) a (B) ====> (A)   (B)
 *  |_____|
 */
Node CombineNode5(char transition)
{
    Node node;
    node.x = GenerateUniqueState(&machine);
    grpInsertVertexL(&machine, node.x);
    node.y = GenerateUniqueState(&machine);
    grpInsertVertexL(&machine, node.y);
    grpInsertEdgeL(&machine, node.x, node.y, (size_t)transition);
    return node;
}
 
/*
 *  ________          __x__  __e__  __y__
 *  |      |         |     V/     V/     V
 * (A) xy (B) ====> (A)   (C)    (D)    (B)
 *  |______|
 */
Node CombineNode1(Node nodeA, Node nodeB)
{
    Node nodeR;
    grpInsertEdgeL(&machine, nodeA.y, nodeB.x, 0); /* Epsilon transition. */
    nodeR.x = nodeA.x;
    nodeR.y = nodeB.y;
    return nodeR;
}
 
/*
 *  _________          __e__  __x__  __e__
 *  |       |         |     V/     V/     V
 * (A) x|y (B) ====> (A)   (C)    (D)    (B)
 *  |_______|         |      __y___       ^
 *                    \     |      |      |
 *                     \e->(E)    (F)--e-/
 */
Node CombineNode2(Node nodeA, Node nodeB)
{
    Node nodeR;
    nodeR.x = GenerateUniqueState(&machine);
    grpInsertVertexL(&machine, nodeR.x);
    nodeR.y = GenerateUniqueState(&machine);
    grpInsertVertexL(&machine, nodeR.y);
 
    grpInsertEdgeL(&machine, nodeR.x, nodeA.x, 0); /* Epsilon transition. */
    grpInsertEdgeL(&machine, nodeR.x, nodeB.x, 0);
    grpInsertEdgeL(&machine, nodeA.y, nodeR.y, 0);
    grpInsertEdgeL(&machine, nodeB.y, nodeR.y, 0);
 
    return nodeR;
}
 
/*
 *  _________          __e__  __x__  __e__
 *  |       |         |     V/     V/     V
 * (A) x|y (B) ====> (A)   (C)    (D)    (B)
 *  |_______|         ^\_______e____>____/|
 *                    \________e____<_____/
 */
Node CombineNode3(Node nodeA)
{
    Node nodeR;
    nodeR.x = GenerateUniqueState(&machine);
    grpInsertVertexL(&machine, nodeR.x);
    nodeR.y = GenerateUniqueState(&machine);
    grpInsertVertexL(&machine, nodeR.y);
 
    grpInsertEdgeL(&machine, nodeR.x, nodeA.x, 0);
    grpInsertEdgeL(&machine, nodeA.y, nodeR.y, 0);
    grpInsertEdgeL(&machine, nodeR.x, nodeR.y, 0);
    grpInsertEdgeL(&machine, nodeR.y, nodeR.x, 0);
 
    return nodeR;
}
 
int cbftvsPrint(void * pitem, size_t param)
{
    P_Element pele = (P_Element)((P_NODE_S)pitem)->pdata;
    if (! pele->isOperator)
        printf(" (%c)", pele->data.operand);
    else
        printf(" %c", pele->data.operator);
    return CBF_CONTINUE;
}
 
//int CBFFindEdgeInList(void * pitem, size_t param)
//{
//  _P_FIEDG pd = (_P_FIEDG)param;
//  if (((P_EDGE)((P_NODE_S)pitem)->pdata)->weight == pd->vertex.weight)
//  {
//      pd->vertex.vid = ((P_EDGE)((P_NODE_S)pitem)->pdata)->vid;
//      pd->pnode = (P_NODE_S)pitem;
//      return CBF_TERMINATE;
//  }
//  return CBF_CONTINUE;
//}
//
//extern P_VERTEX_L _grpGetVertexByID(P_GRAPH_L pgrp, size_t vid);
//
//ptrdiff_t FSA_Status(P_GRAPH_L pgrp, size_t vidx, char transition)
//{
//  REGISTER P_VERTEX_L pvtx;
//  if (NULL != (pvtx = _grpGetVertexByID(pgrp, vidx)))
//  {
//      _FIEDG fd;
//      extern int _grpCBFFindEdgeInList(void * pitem, size_t param);
//      fd.pnode = NULL;
//      fd.vertex.weight = transition;
//
//      if (CBF_TERMINATE == strTraverseLinkedListSC_X(pvtx->adjlist, NULL, CBFFindEdgeInList, (size_t)&fd))
//          return fd.vertex.vid; /* Edge already exists. */
//  }
//  return -1; /* Can not find vertex vidx. */
//}
 
int cbftvsvtxedg(void * pitem, size_t param)
{
    P_NODE_S pnode = (P_NODE_S)pitem;
    P_EDGE pedge = (P_EDGE)pnode->pdata;
    printf("\t\'\\%lu\'\t(%lu)\n", pedge->weight, pedge->vid);
    return CBF_CONTINUE;
}
 
int cbftvsgrp(void * pitem, size_t param)
{
    P_TNODE_B tnodeb = P2P_TNODE_B(pitem);
    P_VERTEX_L pvtx = (P_VERTEX_L)tnodeb->pdata;
    P_Node pnode = (P_Node)param;
    if (pvtx->vid == pnode->x)
        printf(">(%lu)\n", pvtx->vid); /* Start status. */
    else if (pvtx->vid == pnode->y)
        printf("*(%lu)\n", pvtx->vid); /* End status. */
    else
        printf(" (%lu)\n", pvtx->vid);
    strTraverseLinkedListSC_A(pvtx->adjlist, NULL, cbftvsvtxedg, 0);
    return CBF_CONTINUE;
}
 
int main(void)
{
    Node node;
    STACK_L operandStack;
    //char exp[] = { "a(a|b)*" };
    char exp[BUFSIZ] = { 0 };
    stkInitL(&stkOperator);
    stkInitL(&stkOperand);
    queInitL(&queRegexp);
    stkInitL(&operandStack);
    grpInitL(&machine);
 
    printf("Regexp< a(a|d)* >: ");
    scanf_s("%s", exp, BUFSIZ);
    Splitter(exp);
    printf("RPN: ");
    strTraverseLinkedListSC_N(queRegexp.pfront, NULL, cbftvsPrint, 0);
    printf("\n");
 
    while (!queIsEmptyL(&queRegexp))
    {
        Element ele;
        queRemoveL(&ele, sizeof ele, &queRegexp);
        if (ele.isOperator)
        {
            Node nodeA, nodeB;
            switch (ele.data.operator)
            {
            case '|':
                if (!stkIsEmptyL(&operandStack))
                    stkPopL(&nodeA, sizeof nodeA, &operandStack);
                else
                {
                    printf("Expression Error!\n");
                    return 1;
                }
                if (!stkIsEmptyL(&operandStack))
                    stkPopL(&nodeB, sizeof nodeB, &operandStack);
                else
                {
                    printf("Expression Error!\n");
                    return 2;
                }
                nodeA = CombineNode2(nodeA, nodeB);
                stkPushL(&operandStack, &nodeA, sizeof nodeA);
                break;
            case '*':
                if (!stkIsEmptyL(&operandStack))
                    stkPopL(&nodeA, sizeof nodeA, &operandStack);
                else
                {
                    printf("Expression Error!\n");
                    return 3;
                }
                nodeA = CombineNode3(nodeA);
                stkPushL(&operandStack, &nodeA, sizeof nodeA);
                break;
            }
        }
        else
        {
            node = CombineNode5(ele.data.operand);
            stkPushL(&operandStack, &node, sizeof node);
        }
    }
    /* Deduction: */
    while (!stkIsEmptyL(&operandStack))
    {
        Node nodeA, nodeB;
        stkPopL(&nodeA, sizeof nodeA, &operandStack);
        if (!stkIsEmptyL(&operandStack))
            stkPopL(&nodeB, sizeof nodeB, &operandStack);
        else
        {
            stkPushL(&operandStack, &nodeA, sizeof nodeA);
            break;
        }
        nodeA = CombineNode1(nodeA, nodeB);
        stkPushL(&operandStack, &nodeA, sizeof nodeA);
    }
    /* Pour the last item out of the stack. */
    if (!stkIsEmptyL(&operandStack))
        stkPopL(&node, sizeof node, &operandStack);
    else
    {
        printf("Expression Error!\n");
        return 4;
    }
 
    printf("Status\tTransition\tStatus\n");
    treTraverseBIn(machine, cbftvsgrp, &node);
 
    stkFreeL(&stkOperator);
    stkFreeL(&stkOperand);
    queFreeL(&queRegexp);
    stkFreeL(&operandStack);
    grpFreeL(&machine);
    return 0;
}
