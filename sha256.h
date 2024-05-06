/*
 * Name:        sha256.h
 * Description: SHA 256 Algorithm.
 * File ID:     0506241037C0506241037L00017
 *              https://www.programmingalgorithms.com/algorithm/sha256/c/
 * License:     Public Domain.
 */
#ifndef _SHA256_H_
#define _SHA256_H_

#include <stddef.h>
#include <stdint.h>

void sha256(uint8_t hash[32], uint8_t data[], size_t len);

#endif
