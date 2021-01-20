/*
 * Name:        test_svcf.c
 * Description: Launcher of compress/Decompress files.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0120211637C0120211656L00034
 * License:     Public Domain.
 */

#include <stdio.h>
#include <string.h>
#include "svcompress.h"

int main(int argc, char ** argv)
{
	int i;
	for (i = 0; i < argc; ++i)
	{
		if (0 == strcmp(argv[i], "-c") || 0 == strcmp(argv[i], "--compress"))
		{
			return svcCompressFile(stdout, stdin);
		}
		if (0 == strcmp(argv[i], "-d") || 0 == strcmp(argv[i], "--decompress"))
		{
			return svcDecompressFile(stdout, stdin);
		}
		if (0 == strcmp(argv[i], "-?") || 0 == strcmp(argv[i], "--help") || 0 == strcmp(argv[i], "-h"))
		{
			printf("Usage:\n\tsvcf [-c|--compress] Compress stdin.\n\tsvcf [-d|--decompress] Decompress stdin as a svcf file.\n\tsvcf [-?|-h|--help] Show this help content.\n");
			break;
		}
	}
	return 0;
}
