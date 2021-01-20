/*
 * Name:        svcompress.h
 * Description: Compress files.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0120211637A0120211640L00028
 * License:     Public Domain.
 */

#ifndef _SVCOMPRESS_H_
#define _SVCOMPRESS_H_

#include <stdio.h>

typedef enum en_SVCERROR {
	SVC_NONE,       /* No error. */
	SVC_FILE_OPEN,  /* File pointer is NULL. */
	SVC_ALLOCATION, /* Allocation failure. */
	SVC_COMPRESS,   /* Compressing error. */
	SVC_FILE_TYPE,  /* Data file error. */
	SVC_PLATFORM,   /* Platform intger length mismatch. */
	SVC_DECOMPRESS  /* Decompressing error. */
} SVCERROR;

SVCERROR svcCompressFile  (FILE * fpout, FILE * fpin);
SVCERROR svcDecompressFile(FILE * fpout, FILE * fpin);

#endif
