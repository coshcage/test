/*
 * Name:        idea.h
 * Description: International Data Encryption Algorithm.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0506240719B0506240749L00031
 * License:     Public Domain.
 */
#ifndef _IDEA_H_
#define _IDEA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct st_IDEA
{
	uint8_t  bKey  [16]; /* User key of 128 bits. */
	uint16_t bEnKey[53]; /* Encryption key.       */
	uint16_t bDeKey[53]; /* Decryption key.       */
} IDEA, * P_IDEA;

void * ideaSecureZeroMemory(void *    ptr,  size_t    cnt);
void   ideaInit            (P_IDEA    pi,   uint8_t * pKey,    size_t num);
void   ideaFree            (P_IDEA    pi);
P_IDEA ideaCreate          (uint8_t * pKey, size_t    num);
void   ideaDelete          (P_IDEA    pi);
bool   ideaEncrypt         (P_IDEA    pi,   uint8_t * pText,   size_t num, uint8_t * pCipher, size_t * pnum);
bool   ideaDecrypt         (P_IDEA    pi,   uint8_t * pCipher, size_t num, uint8_t * pText,   size_t * pnum);

#endif
