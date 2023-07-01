
/* vrcat for Visual Red Cat.
 * (C) cosh.cage#hotmail.com
 * 06/15/2023
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "svstring.h"

int main(int argc, char * argv[])
{
		size_t i = 0, m;
		ARRAY_Z * parrtxt, arrptn, * parr;
		
		if (argc < 2)
				return 0;
		arrptn.pdata = argv[1];
		arrptn.num = strlen(argv[1]);
		
		if (arrptn.num < 1)
				return 0;

		parrtxt = strCreateArrayZ(BUFSIZ, sizeof(char));
		if (NULL == parrtxt)
				return 0;
		
		while (!feof(stdin))
		{
				parrtxt->pdata[i] = (UCHART)fgetc(stdin);
				if (++i > strLevelArrayZ(parrtxt))
						if (NULL == strResizeArrayZ(parrtxt, strLevelArrayZ(parrtxt) + BUFSIZ, sizeof(char)))
								return 0;
		}
		if (NULL == strResizeArrayZ(parrtxt, i, sizeof(char)))
				return 0;

		parr = strCreateZSearchArrayZ(parrtxt, &arrptn, sizeof(char));

		if (NULL != parr)
		{
				BOOL b = FALSE;
				size_t j, k;

				for (j = i = 0; i < strLevelArrayZ(parrtxt) - 1; ++i)
				{
						if (j[(size_t *)parr->pdata] == i)
						{
								b = TRUE;
								k = 0;
								++j;
						}
						if (b)
								printf("%s", "\e[31m");
						fputc(parrtxt->pdata[i], stdout);
						if (++k >= strLevelArrayZ(&arrptn))
						{
								printf("%s", "\e[0m");
								b = FALSE;
						}
				}
				m = parr->num;
				strDeleteArrayZ(parr);
		}

		strDeleteArrayZ(parrtxt);

		return m;
}
