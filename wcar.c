/*
 * Name:        wcar.c
 * Description: Word correction and recommendation.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0430240345A0430240345L00076
 * License:     Public Domain.
 */
#include <stdio.h>
#include <string.h>
#include "svstring.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))

size_t WagnerFischer(const char * str1, const char * str2)
{
	size_t l1 = strlen(str1);
	size_t l2 = strlen(str2);
	MATRIX dp;

	if (0 == l1)
		return l2;
	if (0 == l2)
		return l1;

	if (NULL != strInitMatrix(&dp, l1 + 1, l2 + 1, sizeof(size_t)))
	{
		size_t i, j;

		for (i = 0; i <= l1; ++i)
			strSetValueMatrix(&dp, i, 0, &i, sizeof(size_t));

		for (j = 0; j <= l2; ++j)
			strSetValueMatrix(&dp, 0, j, &j, sizeof(size_t));

		for (i = 1; i <= l1; ++i)
		{
			char ch1 = str1[i - 1];
			for (j = 1; j <= l2; ++j)
			{
				char ch2 = str2[j - 1];

				size_t k = ch1 == ch2 ? 0 : 1;

				size_t m, n;

				strGetValueMatrix(&m, &dp, i - 1, j, sizeof(size_t));
				++m;
				strGetValueMatrix(&n, &dp, i, j - 1, sizeof(size_t));
				++n;
				m = MIN(m, n);
				strGetValueMatrix(&n, &dp, i - 1, j - 1, sizeof(size_t));
				n += k;
				m = MIN(m, n);

				strSetValueMatrix(&dp, i, j, &m, sizeof(size_t));
			}
		}
		
		strGetValueMatrix(&i, &dp, l1, l2, sizeof(size_t));

		strFreeMatrix(&dp);

		return i;
	}

	return 0;
}

int main()
{
	printf("%d\n", WagnerFischer("cat", "bat"));
	printf("%d\n", WagnerFischer("bat", "ball"));
	
	return 0;
}
