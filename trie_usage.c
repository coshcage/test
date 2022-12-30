#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "svtree.h"
#include "svstring.h"
 
typedef struct st_VOCABULARY
{
    long freq; /* 单词频率 */
    char * pword; /* malloc申请的单词字符串 */
} VOCABULARY, * P_VOCABULARY;
 
P_VOCABULARY CreateWord()
{
    P_VOCABULARY pvol = malloc(sizeof(VOCABULARY));
    if (NULL != pvol)
    {
        pvol->freq = 1;
        pvol->pword = NULL;
    }
    return pvol;
}
 
void DeleteWord(P_VOCABULARY pvol)
{
    free(pvol->pword);
    free(pvol);
}
 
void DestoryArray(P_ARRAY_Z parrz)
{
    size_t i = 0;
    for (; i < parrz->num; ++i)
    {
        P_VOCABULARY pvol = *(((P_VOCABULARY *)parrz->pdata) + i);
        DeleteWord(pvol);
    }
    strFreeArrayZ(parrz);
}
 
int CbfPrintResult(void * pitem, size_t param)
{
    printf("%s %lu\n", (*((P_VOCABULARY *)pitem))->pword, (*((P_VOCABULARY *)pitem))->freq);
    DWC4100(param);
    return CBF_CONTINUE;
}
 
int cbfcmpchar(const void * px, const void * py)
{
    return *(char *)px - *(char *)py;
}
 
int cbfcmpfreq(const void * px, const void * py)
{
    return (*(P_VOCABULARY *)py)->freq - (*(P_VOCABULARY *)px)->freq;
}
 
int main(int argc, char * argv[])
{
    int c;
    long i, j = 0, k = 0, sizeBuffer = 1024, sizeArray = 1024;
    BOOL bCase = FALSE, bSort = FALSE;
    char * buffer = NULL;
    P_TRIE_A ptrie;
    ARRAY_Z arrz;
 
    if (NULL == (buffer = (char *) malloc(sizeBuffer)))
        return 1;
    *buffer = '\0';
    if (NULL == (ptrie = treCreateTrieA()))
        return 2;
    if (NULL == strInitArrayZ(&arrz, sizeArray, sizeof(P_VOCABULARY)))
        return 3;
 
    for (i = 1; i < argc; ++i)
    {
        if (0 == strcmp("-s", argv[i])) /* 排序开关 */
            bSort = TRUE;
        else if (0 == strcmp("-c", argv[i])) /* 大小写开关 */
            bCase = TRUE;
        else
            return 4;
    }
    while (EOF != (c = fgetc(stdin)))
    {
        if (j >= sizeBuffer)
        {
            char * tempBuffer;
            sizeBuffer += 1024;
            if (NULL == (tempBuffer = realloc(buffer, sizeBuffer)))
                return 5;
            buffer = tempBuffer;
        }
        if (c >= 'A' && c <= 'Z')
        {
            *(buffer + j++) = bCase ? (char)c : (char)tolower(c);
        }
        else if (c >= 'a' && c <= 'z')
        {
            *(buffer + j++) = (char)c;
        }
        else
        {
            *(buffer + j) = '\0';
            if (0 != j)
            {
                size_t * presult;
                P_VOCABULARY pvol;
                size_t bufferLen = strlen(buffer);
                /* 在Trie里查询buffer所包含的单词 */
                if (NULL == (presult = treSearchTrieA(ptrie, buffer, bufferLen, sizeof(char), cbfcmpchar)))
                {
                    if (k >= sizeArray)
                    {
                        sizeArray += 1024;
                        strResizeArrayZ(&arrz, sizeArray, sizeof(P_VOCABULARY));
                    }
                    /* 如果没找到就创建一个新单词 */
                    pvol = CreateWord();
                    /* 将新单词插入Trie */
                    treInsertTrieA(ptrie, buffer, bufferLen, sizeof(char), (size_t)pvol, cbfcmpchar);
                    if (NULL == (pvol->pword = malloc(bufferLen + 1)))
                        return 6;
                    memcpy(pvol->pword, buffer, bufferLen + 1);
                    /* 将新单词的指针存储在数组里 */
                    *(((P_VOCABULARY *)arrz.pdata) + k++) = pvol;
                }
                else
                {   /* 如果找到单词，就把Trie里存储的Appendix拿出来转换为P_VOCABULARY指针，并且把频率+1 */
                    pvol = (P_VOCABULARY)*presult;
                    ++pvol->freq;
                }
            }
            j = 0;
 
        }
    }
    strResizeArrayZ(&arrz, k, sizeof(P_VOCABULARY));
    if (bSort)
        strSortArrayZ(&arrz, sizeof(P_VOCABULARY), cbfcmpfreq);
    strTraverseArrayZ(&arrz, sizeof(P_VOCABULARY), CbfPrintResult, 0, FALSE);
    DestoryArray(&arrz);
    treDeleteTrieA(ptrie, sizeof(char));
    free(buffer);
    return 0;
}
