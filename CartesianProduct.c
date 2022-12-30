#include <stdio.h>
#include <string.h>
#include "./StoneValley/src/svstring.h"
 
P_MATRIX CreateCartesianProduct(P_MATRIX pma, P_MATRIX pmb, size_t size)
{
    size_t m, n;
    P_MATRIX pmr = strCreateMatrix(m = pma->ln * pmb->ln, n = pma->col + pma->col, size);
    if (NULL != pmr)
    {
        REGISTER size_t i, x, y;
        for (i = 0, x = 0, y = 0; i < m; ++i)
        {
            /* Fill tuples of pma to pmr. */
            memcpy(strGetValueMatrix(NULL, pmr, i, 0, size), strGetValueMatrix(NULL, pma, x, 0, size), size * pma->col);
            /* Fill tuples of pmb to pmr. */
            memcpy(strGetValueMatrix(NULL, pmr, i, pma->col, size), strGetValueMatrix(NULL, pmb, y, 0, size), size * pmb->col);
            if (0 == (i + 1) % pmb->ln)
                ++x;
            if (++y == pmb->ln)
                y = 0;
        }
    }
    return pmr;
}
 
int cbftvs(void * pitem, size_t size)
{
    static i = 1;
    printf("%s ", (char *)pitem);
    if (0 == i % size)
        printf("\n");
    ++i;
    return CBF_CONTINUE;
}
 
int main()
{
    P_MATRIX pmb = NULL, pmc = NULL;
    P_MATRIX pma = strCreateMatrix(3, 3, sizeof(char *));
    strSetValueMatrix(pma, 0, 0, "a1", sizeof(char *));
    strSetValueMatrix(pma, 0, 1, "b1", sizeof(char *));
    strSetValueMatrix(pma, 0, 2, "c1", sizeof(char *));
 
    strSetValueMatrix(pma, 1, 0, "a1", sizeof(char *));
    strSetValueMatrix(pma, 1, 1, "b2", sizeof(char *));
    strSetValueMatrix(pma, 1, 2, "c2", sizeof(char *));
 
    strSetValueMatrix(pma, 2, 0, "a2", sizeof(char *));
    strSetValueMatrix(pma, 2, 1, "b2", sizeof(char *));
    strSetValueMatrix(pma, 2, 2, "c1", sizeof(char *));
 
    pmb = strCreateMatrix(3, 3, sizeof(char *));
    strSetValueMatrix(pmb, 0, 0, "a1", sizeof(char *));
    strSetValueMatrix(pmb, 0, 1, "b2", sizeof(char *));
    strSetValueMatrix(pmb, 0, 2, "c2", sizeof(char *));
 
    strSetValueMatrix(pmb, 1, 0, "a1", sizeof(char *));
    strSetValueMatrix(pmb, 1, 1, "b3", sizeof(char *));
    strSetValueMatrix(pmb, 1, 2, "c2", sizeof(char *));
 
    strSetValueMatrix(pmb, 2, 0, "a2", sizeof(char *));
    strSetValueMatrix(pmb, 2, 1, "b2", sizeof(char *));
    strSetValueMatrix(pmb, 2, 2, "c1", sizeof(char *));
 
    pmc = CreateCartesianProduct(pma, pmb, sizeof(char *));
 
    strTraverseArrayZ(&pma->arrz, sizeof(char *), cbftvs, 3, FALSE);
    printf("\n");
    strTraverseArrayZ(&pmb->arrz, sizeof(char *), cbftvs, 3, FALSE);
    printf("\n");
 
    strTraverseArrayZ(&pmc->arrz, sizeof(char *), cbftvs, 6, FALSE);
 
    strDeleteMatrix(pma);
    strDeleteMatrix(pmb);
    strDeleteMatrix(pmc);
    return 0;
}
