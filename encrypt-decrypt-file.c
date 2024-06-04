#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "idea.h"
#include "sha256.h"
#include "svstring.h"

#define SZSALT "\xeb\xeb\xeb\xeb"

P_ARRAY_Z ReadFileIntoArrayZ(FILE * fp, bool bdirect)
{
	P_ARRAY_Z parr;
	size_t i = bdirect ? 0 : sizeof(size_t);

	if (NULL != (parr = strCreateArrayZ(BUFSIZ, sizeof(char))))
	{
		while (!feof(fp))
		{
			parr->pdata[i] = (char)fgetc(fp);
			++i;
			if (i >= strLevelArrayZ(parr))
				if (NULL == strResizeArrayZ(parr, strLevelArrayZ(parr) + BUFSIZ, sizeof(char)))
					return NULL;
		}
		if (!bdirect)
			0[(size_t *)parr->pdata] = i - 1;
		if (NULL == strResizeArrayZ(parr, i - 1, sizeof(char)))
			return NULL;
	}
	return parr;
}

bool EncryptFile(FILE * pfout, FILE * pfin)
{
	if (NULL == pfout || NULL == pfin)
		return false;
	else
	{
		IDEA idea;
		size_t i, j;
		int c;
		char password[BUFSIZ] = { 0 };
		char tempbuff[BUFSIZ * 2] = { 0 };
		unsigned char hash[32];
		unsigned char hash2[32];
		P_ARRAY_Z parr = ReadFileIntoArrayZ(pfin, false);
		P_ARRAY_Z parr2 = strCreateArrayZ(strLevelArrayZ(parr) + BUFSIZ, sizeof(char));
		do
		{
			printf("Password: ");
			for (i = 0;;)
			{
				c = getchar();
				if ('\n' == c || '\r' == c)
					break;
				if ('\b' == c)
					--i;
				if (i >= BUFSIZ)
					break;
				password[i] = (char)c;
				++i;
			}
			password[i] = 0;
			printf("Input password again: ");
			for (i = 0;;)
			{
				c = getchar();
				if ('\n' == c || '\r' == c)
					break;
				if ('\b' == c)
					--i;
				if (i >= BUFSIZ)
					break;
				tempbuff[i] = (char)c;
				++i;
			}
			password[i] = 0;
		} while (0 != strcmp(tempbuff, password));

		strcat(tempbuff, SZSALT);
		sha256(hash2, (uint8_t *)tempbuff, strlen(tempbuff));
		ideaInit(&idea, hash2, sizeof(hash2));
		sha256(hash, (uint8_t *)password, strlen(password));

		i = strLevelArrayZ(parr2);

		ideaEncrypt(&idea, parr->pdata, strLevelArrayZ(parr), parr2->pdata, &i);

		fwrite(hash, sizeof(char), sizeof(hash), pfout);

		for (j = 0; j < i; ++j)
			fputc(parr2->pdata[j], pfout);

		ideaSecureZeroMemory(password, BUFSIZ);
		ideaSecureZeroMemory(tempbuff, BUFSIZ);
		ideaSecureZeroMemory(hash, sizeof(hash));
		ideaSecureZeroMemory(hash2, sizeof(hash2));
		strDeleteArrayZ(parr);
		strDeleteArrayZ(parr2);
	}
	return true;
}

bool DecryptFile(FILE * pfout, FILE * pfin)
{
	if (NULL == pfout || NULL == pfin)
		return false;
	else
	{
		IDEA idea;
		size_t i, j;
		int c;
		char password[BUFSIZ] = { 0 };
		char tempbuff[BUFSIZ * 2] = { 0 };
		unsigned char hash[32];
		unsigned char hash2[32];
		P_ARRAY_Z parr;
		P_ARRAY_Z parr2;

		fread(hash2, sizeof(char), sizeof(hash2), pfin);
		if (feof(pfin))
		{
			printf("File type error.\n");
			return false;
		}

		printf("Password: ");
		for (i = 0;;)
		{
			c = getchar();
			if ('\n' == c || '\r' == c)
				break;
			if ('\b' == c)
				--i;
			if (i >= BUFSIZ)
				break;
			password[i] = (char)c;
			++i;
		}
		password[i] = 0;
		sha256(hash, (uint8_t *)password, strlen(password));
		if (0 != memcmp(hash, hash2, sizeof(hash)))
		{
			printf("\nWrong password! Access denied.\n");
			return false;
		}

		memcpy(tempbuff, password, strlen(password) + 1);
		strcat(tempbuff, SZSALT);
		sha256(hash2, (uint8_t *)tempbuff, strlen(tempbuff));

		ideaInit(&idea, hash2, sizeof(hash2));

		parr = ReadFileIntoArrayZ(pfin, true);
		parr2 = strCreateArrayZ(strLevelArrayZ(parr) + BUFSIZ, sizeof(char));

		i = strLevelArrayZ(parr2);

		ideaDecrypt(&idea, parr->pdata, strLevelArrayZ(parr), parr2->pdata, &i);

		i = 0[(size_t *)parr2->pdata];

		for (j = sizeof(size_t); j < i; ++j)
			fputc(parr2->pdata[j], pfout);

		ideaSecureZeroMemory(password, BUFSIZ);
		ideaSecureZeroMemory(tempbuff, BUFSIZ);
		ideaSecureZeroMemory(hash, sizeof(hash));
		ideaSecureZeroMemory(hash2, sizeof(hash2));
		strDeleteArrayZ(parr);
		strDeleteArrayZ(parr2);
	}
	return true;
}

int main(int argc, char ** argv)
{
	bool r = false;
	char buf[L_tmpnam] = { 0 };
	FILE * pfin, * pfout;

	if (argc != 3)
	{
		printf("Arguments error.\n");
		return 1;
	}
	if (!strcmp(argv[1], "-e"))
	{
		pfin = fopen(argv[2], "rb");
		pfout = fopen("encrypt.out", "wb+");

		r = EncryptFile(pfout, pfin);

		fclose(pfin);
		fclose(pfout);
	}
	else if (!strcmp(argv[1], "-d"))
	{
		pfin = fopen(argv[2], "rb");
		pfout = fopen("decrypt.out", "wb+");

		r = DecryptFile(pfout, pfin);

		fclose(pfin);
		fclose(pfout);
	}
	else
	{
		printf("./a.out -e filename // Encrypt.\n./a.out -d filename // Decrypt.\n");
		return 1;
	}

	return r;
}
