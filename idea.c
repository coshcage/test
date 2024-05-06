/*
 * Name:        idea.c
 * Description: International Data Encryption Algorithm.
 * Author:      cosh.cage#hotmail.com
 *              This code is duplicated from book ISBN 978-7-111-23862-1.
 * File ID:     0506240719A0506240748L00409
 * License:     Public Domain.
 */

#include <stdlib.h> /* Using function malloc, free. */
#include <string.h> /* Using function memcpy, memset. */
#include "idea.h"

/* Useful macros to do arithmetic. */
#define XOR(a, b) ((a) ^ (b))
#define ADD(a, b) ((uint16_t)(((uint32_t)(a) + (uint32_t)(b)) & 0xFFFF))
#define MUL(a, b) ((uint16_t)(((uint32_t)(a) * (uint32_t)(b)) % 0x10001))

/* Used for decryption. */
size_t gMap[53] =
{
	0, 49, 50, 51, 52, 47, 48,
	43, 45, 44, 46, 41, 42,
	37, 39, 38, 40, 35, 36,
	31, 33, 32, 34, 29, 30,
	25, 27, 26, 28, 23, 24,
	19, 21, 20, 22, 17, 18,
	13, 15, 14, 16, 11, 12,
	7,  9,  8,  10,  5,  6,
	1,  2,  3,  4
};

/* Used for decryption. */
ptrdiff_t gFlag[53] =
{
	0, 1, -1, -1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, -1, -1, 1, 0, 0,
	1, -1, -1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, -1, -1, 1, 0, 0, 1, -1, -1, 1
};

/* Used for generate encryption key. */
uint8_t gBitMask[8] =
{
	128, 64, 32, 16, 8, 4, 2, 1
};

/* Inline macro to accelerate execution speed. */
#define GetBitByIndex(bKey, i) (((bKey)[(i) / 8] & gBitMask[(i) & 0x7]) ? 1 : 0)

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: ExtendedEuclid
 * Description:   Do extended Euclid Algorithm.
 * Parameters:
 *          e Sized integer parameter.
 *          f Sized integer parameter.
 * Return value:  A sized integer parameter.
 */
static ptrdiff_t ExtendedEuclid(ptrdiff_t e, ptrdiff_t f)
{
	register ptrdiff_t x1 = 1, x2 = 0, x3 = f;
	register ptrdiff_t y1 = 0, y2 = 1, y3 = e;
	if (0 == x3 || 0 == y3)
		return 0;
	if (1 == y3)
		return y2;
	for (;;)
	{
		register ptrdiff_t b = x3 / y3;
		register ptrdiff_t t1 = x1 - y1 * b;
		register ptrdiff_t t2 = x2 - y2 * b;
		register ptrdiff_t t3 = x3 - y3 * b;
		x1 = y1; x2 = y2; x3 = y3;
		y1 = t1; y2 = t2; y3 = t3;
		if (1 == y3)
		{
			if (y2 > 0)
				return y2;
			else
				return y2 + f;
		}
	}
}

/* Function name: ideaSecureZeroMemory
 * Description:   Secure set the memory to zero.
 * Parameters:
 *        ptr Pointer to a memory address.
 *        cnt Length of the memory by bytes.
 * Return value:  The same address of parameter ptr.
 */
void * ideaSecureZeroMemory(void * ptr, size_t cnt)
{
	volatile char * vptr = (volatile char *)ptr;
	while (cnt)
	{
		*vptr = 0;
		++vptr;
		--cnt;
	}
	return ptr;
}

/* Function name: ideaInit
 * Description:   Initialize IDEA structure and generate keys.
 * Parameters:
 *         pi Pointer to an IDEA structure.
 *       pKey Pointer to a narrow string of password.
 *        num Length of the password.
 * Return value:  N/A.
 */
void ideaInit(P_IDEA pi, uint8_t * pKey, size_t num)
{
	register size_t i, j, k = 1;

	memcpy(pi->bKey, pKey, (num < 16 ? num : 16));

	/* Generate encryption key. */
	for (i = 1; i <= 52; ++i)
	{
		for (j = 0; j < 16; ++j)
		{
			pi->bEnKey[i] <<= 1;
			pi->bEnKey[i] += GetBitByIndex(pi->bKey, (k - 1) & 0x7F);
			++k;
		}
		if ((i % 8) == 0)
			k += 25;
	}

	/* Generate decryption key. */
	for (i = 1; i <= 52; ++i)
	{
		switch (gFlag[i])
		{
		case 1:
			pi->bDeKey[i] = (uint16_t)ExtendedEuclid(pi->bEnKey[gMap[i]], 0x10001);
			break;
		case 0:
			pi->bDeKey[i] = pi->bEnKey[gMap[i]];
			break;
		case -1:
			pi->bDeKey[i] = -1 * pi->bEnKey[gMap[i]];
			break;
		}
	}
}

/* Function name: ideaFree
 * Description:   Free IDEA structure and clean keys.
 * Parameter:
 *        pi Pointer to an IDEA structure.
 * Return value:  N/A.
 */
void ideaFree(P_IDEA pi)
{
	ideaSecureZeroMemory(pi, sizeof(IDEA));
}

/* Function name: ideaCreate
 * Description:   Create an IDEA structure and generate keys.
 * Parameters:
 *       pKey Pointer to a narrow string of password.
 *        num Length of the password.
 * Return value:  Pointer to an IDEA structure.
 */
P_IDEA ideaCreate(uint8_t * pKey, size_t num)
{
	register P_IDEA pi;
	if (0 == num)
		return NULL;
	pi = (P_IDEA)malloc(sizeof(IDEA));
	if (NULL != pi)
		ideaInit(pi, pKey, num);

	return pi;
}

/* Function name: ideaDelete
 * Description:   Delete an IDEA structure and clean keys.
 * Parameter:
 *        pi Pointer to an IDEA structure.
 * Return value:  N/A.
 */
void ideaDelete(P_IDEA pi)
{
	ideaFree(pi);
	free(pi);
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: EncryptionRoundTransform
 * Description:   Do a round of encryption.
 * Parameters:
 *         pi Pointer to an IDEA structure.
 *        px1 Pointer to a uint16_t integer.
 *        px2 Pointer to a uint16_t integer.
 *        px3 Pointer to a uint16_t integer.
 *        px4 Pointer to a uint16_t integer.
 *      index An uint16_t integer.
 * Return value:  N/A.
 */
static void EncryptionRoundTransform(P_IDEA pi, uint16_t * px1, uint16_t * px2, uint16_t * px3, uint16_t * px4, size_t index)
{
	if (index > 9)
		return;
	else if (9 == index)
	{
		*px1 = MUL(*px1, pi->bEnKey[49]);
		*px2 = ADD(*px2, pi->bEnKey[50]);
		*px3 = ADD(*px3, pi->bEnKey[51]);
		*px4 = MUL(*px4, pi->bEnKey[52]);
		return;
	}
	{
		register uint16_t k1 = pi->bEnKey[(index - 1) * 6 + 1];
		register uint16_t k2 = pi->bEnKey[(index - 1) * 6 + 2];
		register uint16_t k3 = pi->bEnKey[(index - 1) * 6 + 3];
		register uint16_t k4 = pi->bEnKey[(index - 1) * 6 + 4];
		register uint16_t k5 = pi->bEnKey[(index - 1) * 6 + 5];
		register uint16_t k6 = pi->bEnKey[(index - 1) * 6 + 6];

		register uint16_t v1 = MUL(*px1, k1);
		register uint16_t v2 = ADD(*px2, k2);
		register uint16_t v3 = ADD(*px3, k3);
		register uint16_t v4 = MUL(*px4, k4);
		register uint16_t v5 = XOR(v1, v3);
		register uint16_t v6 = XOR(v2, v4);
		register uint16_t v7 = MUL(v5, k5);
		register uint16_t v8 = ADD(v6, v7);
		register uint16_t v9 = MUL(v8, k6);
		register uint16_t v10 = ADD(v7, v9);
		register uint16_t v11 = XOR(v1, v9);
		register uint16_t v12 = XOR(v3, v9);
		register uint16_t v13 = XOR(v2, v10);
		register uint16_t v14 = XOR(v4, v10);

		*px1 = v11;
		*px2 = v12;
		*px3 = v13;
		*px4 = v14;
	}
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: DecryptionRoundTransform
 * Description:   Do a round of decryption.
 * Parameters:
 *         pi Pointer to an IDEA structure.
 *        px1 Pointer to a uint16_t integer.
 *        px2 Pointer to a uint16_t integer.
 *        px3 Pointer to a uint16_t integer.
 *        px4 Pointer to a uint16_t integer.
 *      index An uint16_t integer.
 * Return value:  N/A.
 */
static void DecryptionRoundTransform(P_IDEA pi, uint16_t * px1, uint16_t * px2, uint16_t * px3, uint16_t * px4, size_t index)
{
	if (index > 9)
		return;
	else if (9 == index)
	{
		*px1 = MUL(*px1, pi->bDeKey[49]);
		*px2 = ADD(*px2, pi->bDeKey[50]);
		*px3 = ADD(*px3, pi->bDeKey[51]);
		*px4 = MUL(*px4, pi->bDeKey[52]);
		return;
	}
	{
		register uint16_t k1 = pi->bDeKey[(index - 1) * 6 + 1];
		register uint16_t k2 = pi->bDeKey[(index - 1) * 6 + 2];
		register uint16_t k3 = pi->bDeKey[(index - 1) * 6 + 3];
		register uint16_t k4 = pi->bDeKey[(index - 1) * 6 + 4];
		register uint16_t k5 = pi->bDeKey[(index - 1) * 6 + 5];
		register uint16_t k6 = pi->bDeKey[(index - 1) * 6 + 6];

		register uint16_t v1 = MUL(*px1, k1);
		register uint16_t v2 = ADD(*px2, k2);
		register uint16_t v3 = ADD(*px3, k3);
		register uint16_t v4 = MUL(*px4, k4);
		register uint16_t v5 = XOR(v1, v3);
		register uint16_t v6 = XOR(v2, v4);
		register uint16_t v7 = MUL(v5, k5);
		register uint16_t v8 = ADD(v6, v7);
		register uint16_t v9 = MUL(v8, k6);
		register uint16_t v10 = ADD(v7, v9);
		register uint16_t v11 = XOR(v1, v9);
		register uint16_t v12 = XOR(v3, v9);
		register uint16_t v13 = XOR(v2, v10);
		register uint16_t v14 = XOR(v4, v10);

		*px1 = v11;
		*px2 = v12;
		*px3 = v13;
		*px4 = v14;
	}
}

/* Function name: ideaEncrypt
 * Description:   IDEA encryption.
 * Parameters:
 *         pi Pointer to an initialized IDEA structure.
 *      pText Pointer to a narrow string that you want to encrypt.
 *        num Length of text of pText.
 *    pCipher Pointer to the byte array to be encrypted.
 *       pnum Input an address of a size_t integer that is the length of pCipher buffer.
 *            Output the buffer length of pCipher that is encrypted.
 * Return value:  true:  Encryption succeeded.
 *                false: Encryption failed.
 * Tip:           //#include <stdio.h>
 *                //#include "idea.h"
 *                //int main() {
 *                //	size_t i = 100;
 *                //	char text[100] = "hello world!";
 *                //	char cipher[100] = { 0 };
 *                //	P_IDEA p;
 *                //	p = ideaCreate("password", 8);
 *                //	ideaEncrypt(p, text, 13, cipher, &i);
 *                //	ideaDecrypt(p, cipher, i, text, &i);
 *                //	printf("%s\n", text);
 *                //	ideaDelete(p);
 *                //	return 0;
 *                //}
 */
bool ideaEncrypt(P_IDEA pi, uint8_t * pText, size_t num, uint8_t * pCipher, size_t * pnum)
{
	size_t nsize = (num % 8) ? (num / 8 * 8 + 8) : num;
	if (*pnum < nsize)
		return false;

	memset(pCipher, 0, *pnum);
	memcpy(pCipher, pText, nsize);
	{
		uint16_t a, b, c, d;
		register size_t nTime = nsize / 8, i, j;
		for (i = 0; i < nTime; ++i)
		{
			a = (pCipher[8 * i] << 8) + pCipher[8 * i + 1];
			b = (pCipher[8 * i + 2] << 8) + pCipher[8 * i + 3];
			c = (pCipher[8 * i + 4] << 8) + pCipher[8 * i + 5];
			d = (pCipher[8 * i + 6] << 8) + pCipher[8 * i + 7];

			for (j = 1; j <= 8; ++j)
				EncryptionRoundTransform(pi, &a, &b, &c, &d, j);
			EncryptionRoundTransform(pi, &a, &c, &b, &d, 9);

			pCipher[8 * i] = (uint8_t)(a >> 8);
			pCipher[8 * i + 1] = (a & 0xFF);
			pCipher[8 * i + 2] = (uint8_t)(c >> 8);
			pCipher[8 * i + 3] = (c & 0xFF);
			pCipher[8 * i + 4] = (uint8_t)(b >> 8);
			pCipher[8 * i + 5] = (b & 0xFF);
			pCipher[8 * i + 6] = (uint8_t)(d >> 8);
			pCipher[8 * i + 7] = (d & 0xFF);
		}
		if (NULL != pnum)
			*pnum = nsize;
	}
	return true;
}

/* Function name: ideaDecrypt
 * Description:   IDEA decryption.
 * Parameters:
 *         pi Pointer to an initialized IDEA structure.
 *    pCipher Pointer to the byte array to be decrypted.
 *        num Length of the array buffer of pCipher.
 *      pText Pointer to the text that is decrypted.
 *       pnum Input an address of a size_t integer that is the length of pText buffer.
 *            Output the buffer length of pText that is decrypted.
 * Return value:  true:  Decryption succeeded.
 *                false: Decryption failed.
 * Tip:           Please reference to function ideaEncrypt.
 */
bool ideaDecrypt(P_IDEA pi, uint8_t * pCipher, size_t num, uint8_t * pText, size_t * pnum)
{
	size_t nsize = (num % 8) ? (num / 8 * 8 + 8) : num;
	if (*pnum < nsize)
		return false;

	memset(pText, 0, *pnum);
	memcpy(pText, pCipher, nsize);
	{
		uint16_t a, b, c, d;
		register size_t nTime = nsize / 8, i, j;
		for (i = 0; i < nTime; i++)
		{
			a = (pText[8 * i] << 8) + pText[8 * i + 1];
			b = (pText[8 * i + 2] << 8) + pText[8 * i + 3];
			c = (pText[8 * i + 4] << 8) + pText[8 * i + 5];
			d = (pText[8 * i + 6] << 8) + pText[8 * i + 7];

			for (j = 1; j <= 8; j++)
				DecryptionRoundTransform(pi, &a, &b, &c, &d, j);
			DecryptionRoundTransform(pi, &a, &c, &b, &d, 9);

			pText[8 * i] = (uint8_t)(a >> 8);
			pText[8 * i + 1] = (a & 0xFF);
			pText[8 * i + 2] = (uint8_t)(c >> 8);
			pText[8 * i + 3] = (c & 0xFF);
			pText[8 * i + 4] = (uint8_t)(b >> 8);
			pText[8 * i + 5] = (b & 0xFF);
			pText[8 * i + 6] = (uint8_t)(d >> 8);
			pText[8 * i + 7] = (d & 0xFF);
		}
		if (NULL != pnum)
			*pnum = nsize;
	}
	return true;
}
