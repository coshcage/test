#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "StoneValley/src/svstring.h"

#define MYTYPE int
#define SIZE (1024 * 1024 * 128)

int cbfcmp(const void * px, const void * py)
{
	return *(MYTYPE *)px - *(MYTYPE *)py;
}

MYTYPE arr[SIZE];

int main(void)
{
	size_t i;
	LARGE_INTEGER la, lb, lc;

	srand(1);
	for (i = 0; i < SIZE; ++i)
		arr[i] = rand();
	QueryPerformanceFrequency(&lc);
	QueryPerformanceCounter(&la);
	svQuickSort(arr, SIZE, sizeof(MYTYPE), cbfcmp);
	QueryPerformanceCounter(&lb);
	printf("%lf\n", (double)(lb.QuadPart - la.QuadPart) / (double)lc.QuadPart);

	for (i = 0; i < SIZE - 1; ++i)
		if (arr[i] > arr[i + 1])
			return 1;

	srand(1);
	for (i = 0; i < SIZE; ++i)
		arr[i] = rand();
	QueryPerformanceFrequency(&lc);
	QueryPerformanceCounter(&la);
	qsort(arr, SIZE, sizeof(MYTYPE), cbfcmp);
	QueryPerformanceCounter(&lb);
	printf("%lf\n", (double)(lb.QuadPart - la.QuadPart) / (double)lc.QuadPart);

	srand(1);
	for (i = 0; i < SIZE; ++i)
		arr[i] = rand();
	QueryPerformanceFrequency(&lc);
	QueryPerformanceCounter(&la);
	svMergeSort(arr, SIZE, sizeof(MYTYPE), cbfcmp);
	QueryPerformanceCounter(&lb);
	printf("%lf\n", (double)(lb.QuadPart - la.QuadPart) / (double)lc.QuadPart);

	for (i = 0; i < SIZE - 1; ++i)
		if (arr[i] > arr[i + 1])
			return 1;

	printf("Finish.\n");
	return 0;
}
