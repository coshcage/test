/*
 * Name:        svcompress.c
 * Description: Compress files.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0120211637B0513250851L00217
 * License:     Public Domain.
 */

#include <stdlib.h>
#include <string.h>
#include "svcompress.h"
#include "svtree.h"

/* Buffer size incremental. */
#define INITIAL_BUFFER_SIZE (1024)

/* Magical number that used to distinguish file type. */
UCHART magicNumber[] = { 'S', 'V', 'C', 'F' };

/* SVCF_File_structure:_________________
 * |Length:  |Name:                    |
 * |---------|-------------------------|
 * |UCHART[4]|Magical number.          |
 * |UCHART   |Platform integer length. |
 * |UCHART   |Symbol table length.     |
 * |N/A      |Symbol table.            |
 * |         |                         |
 * |size_t   |Compressed data length.  |
 * |UCHART   |Remaining bits.          |
 * |N/A      |Compressed data.         |
 * |_________|_________________________|
 */

 /* Function name: svcCompressFile
  * Description:   Compress a file to a file.
  * Parameters:
  *      fpout Pointer to the output file.
  *       fpin Pointer to the input file.
  * Return value:  Error code.
  *                Please refer to the SVCERROR enumeration at file 'svcompress.h'.
  */
SVCERROR svcCompressFile(FILE * fpout, FILE * fpin)
{
	int c;
	UCHART uc;
	size_t i = 0, j, k;
	P_BITSTREAM pbstm;
	ARRAY_Z arrInBuffer, * parrTable = NULL;
	SVCERROR rtn = SVC_NONE;

	if (NULL == fpin || NULL == fpout)
		return SVC_FILE_OPEN;
	if (NULL == strInitArrayZ(&arrInBuffer, INITIAL_BUFFER_SIZE, sizeof(UCHART)))
		return SVC_ALLOCATION;
	/* Read fpin into buffer in the memory. */
	while ((c = fgetc(fpin)) != EOF)
	{
		arrInBuffer.pdata[i] = (UCHART)c;
		if (++i >= strLevelArrayZ(&arrInBuffer))
		{
			if (NULL == strResizeArrayZ(&arrInBuffer, strLevelArrayZ(&arrInBuffer) + INITIAL_BUFFER_SIZE, sizeof(UCHART)))
			{
				rtn = SVC_ALLOCATION;
				goto Lbl_Compress_Error;
			}
		}
	}
	if (NULL == strResizeArrayZ(&arrInBuffer, i, sizeof(UCHART)))
	{
		rtn = SVC_ALLOCATION;
		goto Lbl_Compress_Error;
	}

	/* Compress data. */
	if (NULL == (parrTable = treCreateHuffmanTable(arrInBuffer.pdata, i)))
	{
		rtn = SVC_COMPRESS;
		goto Lbl_Compress_Error;
	}
	if (NULL == (pbstm = treHuffmanEncoding(parrTable, arrInBuffer.pdata, i)))
	{
		rtn = SVC_COMPRESS;
		goto Lbl_Compress_Error;
	}
	if (NULL == parrTable)
	{	/* Can not output symbol table. */
		strDeleteBitStream(pbstm);
		rtn = SVC_COMPRESS;
		goto Lbl_Compress_Error;
	}
	/* Free in-buffer-array. */
	strFreeArrayZ(&arrInBuffer);

	/*
	{
		P_BITSTREAM pbsout;
		pbsout = treHuffmanDecoding(parrTable, pbstm);
		strDeleteBitStream(pbsout);
	}
	*/

	/* Write magical number. */
	fwrite(magicNumber, sizeof magicNumber / sizeof magicNumber[0], sizeof magicNumber[0], fpout);
	/* Write file header which is a UCHART variable that indicates platform integer length. */
	uc = sizeof(size_t);
	fwrite(&uc, sizeof uc, 1, fpout);
	/* Write symbol table length. */
	uc = (UCHART)parrTable->num;
	fwrite(&uc, sizeof uc, 1, fpout);
	/* Write symbol table. */
	k = strLevelArrayZ(parrTable) * sizeof(HFM_SYMBOL);
	for (j = 0; j < k; ++j)
		fputc(parrTable->pdata[j], fpout);
	/* Delete symbol table. */
	strDeleteArrayZ(parrTable);
	/* Write compressed data length. */
	j = strLevelArrayZ(&(pbstm->arrz));
	fwrite(&j, sizeof j, 1, fpout);
	/* Write remaining bits. */
	uc = (UCHART)pbstm->bilc;
	fwrite(&uc, sizeof uc, 1, fpout);
	/* Write compressed data. */
	for (j = 0; j < strLevelArrayZ(&(pbstm->arrz)); ++j)
		fputc(pbstm->arrz.pdata[j], fpout);
	/* Cleanup. */
	strDeleteBitStream(pbstm);
	return SVC_NONE;

Lbl_Compress_Error:
	if (NULL != pbstm)
		strDeleteBitStream(pbstm);
	if (NULL != parrTable)
		strDeleteArrayZ(parrTable);
	strFreeArrayZ(&arrInBuffer);
	return rtn;
}

/* Function name: svcDecompressFile
 * Description:   Decompress a file to a file.
 * Parameters:
 *      fpout Pointer to the output file.
 *       fpin Pointer to the input file.
 * Return value:  Error code.
 *                Please refer to the SVCERROR enumeration at file 'svcompress.h'.
 */
SVCERROR svcDecompressFile(FILE * fpout, FILE * fpin)
{
	int c;
	UCHART uc;
	size_t i, j, k;
	P_ARRAY_Z parrTable;
	BITSTREAM bsin, * pbsout;
	UCHART magicBuffer[sizeof magicNumber / sizeof magicNumber[0]];

	if (NULL == fpin || NULL == fpout)
		return SVC_FILE_OPEN;
	/* Read magical number. */
	fread(magicBuffer, sizeof magicNumber[0], sizeof magicNumber / sizeof magicNumber[0], fpin);
	if (0 != memcmp(magicBuffer, magicNumber, sizeof magicNumber))
		return SVC_FILE_TYPE;
	/* Read platform length. */
	fread(&uc, sizeof(UCHART), 1, fpin);
	if (sizeof(size_t) != (size_t)uc)
		return SVC_PLATFORM;
	/* Read symbol table length. */
	fread(&uc, sizeof(UCHART), 1, fpin);
	j = uc * sizeof(HFM_SYMBOL);
	/* Read symbol table. */
	parrTable = strCreateArrayZ(uc, sizeof(HFM_SYMBOL));
	for (i = 0; i < j; ++i)
	{
		if ((c = fgetc(fpin)) == EOF)
		{
			strDeleteArrayZ(parrTable);
			return SVC_FILE_TYPE;
		}
		parrTable->pdata[i] = (UCHART)c;
	}
	/* Read compressed data length. */
	fread(&k, sizeof(size_t), 1, fpin);
	/* Read remaining bits. */
	fread(&uc, sizeof uc, 1, fpin);
	bsin.bilc = (size_t)uc;
	if (NULL == strInitArrayZ(&bsin.arrz, k, sizeof(UCHART)))
	{
		strDeleteArrayZ(parrTable);
		return SVC_ALLOCATION;
	}
	/* Read compressed data. */
	for (i = 0; i < k; ++i)
	{
		if ((c = fgetc(fpin)) == EOF)
		{
			strDeleteArrayZ(parrTable);
			strFreeBitStream(&bsin);
			return SVC_FILE_TYPE;
		}
		bsin.arrz.pdata[i] = (UCHART)c;
	}
	/* Decompress. */
	if (NULL == (pbsout = treHuffmanDecoding(parrTable, &bsin)))
	{
		strDeleteArrayZ(parrTable);
		strFreeBitStream(&bsin);
		return SVC_DECOMPRESS;
	}
	/* Cleanup. */
	strDeleteArrayZ(parrTable);
	strFreeBitStream(&bsin);
	/* Output result. */
	for (i = 0; i < pbsout->arrz.num; ++i)
		fputc(pbsout->arrz.pdata[i], fpout);
	/* Cleanup. */
	strDeleteBitStream(pbsout);
	return SVC_NONE;
}

