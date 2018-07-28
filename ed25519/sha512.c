/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */
#include <stdint.h>
#include <string.h>
#include "sha512.h"

#define BLOCK_LEN 128  // In bytes
#define STATE_LEN 8  // In words


// Link this program with an external C or x86 compression function
extern void sha512_compress(uint64_t state[static STATE_LEN], const uint8_t block[static BLOCK_LEN]);

void crypto_hash_sha512(unsigned char* const out, const unsigned char* message, size_t len) {
    uint64_t* const hash = (uint64_t *)out;
	hash[0] = UINT64_C(0x6A09E667F3BCC908);
	hash[1] = UINT64_C(0xBB67AE8584CAA73B);
	hash[2] = UINT64_C(0x3C6EF372FE94F82B);
	hash[3] = UINT64_C(0xA54FF53A5F1D36F1);
	hash[4] = UINT64_C(0x510E527FADE682D1);
	hash[5] = UINT64_C(0x9B05688C2B3E6C1F);
	hash[6] = UINT64_C(0x1F83D9ABFB41BD6B);
	hash[7] = UINT64_C(0x5BE0CD19137E2179);

	#define LENGTH_SIZE 16  // In bytes

	size_t off;
	for (off = 0; len - off >= BLOCK_LEN; off += BLOCK_LEN)
		sha512_compress(hash, &message[off]);

	uint8_t block[BLOCK_LEN] = {0};
	size_t rem = len - off;
	memcpy(block, &message[off], rem);

	block[rem] = 0x80;
	rem++;
	if (BLOCK_LEN - rem < LENGTH_SIZE) {
		sha512_compress(hash, block);
		memset(block, 0, sizeof(block));
	}

	block[BLOCK_LEN - 1] = (uint8_t)((len & 0x1FU) << 3);
	len >>= 5;
	for (int i = 1; i < LENGTH_SIZE; i++, len >>= 8)
		block[BLOCK_LEN - 1 - i] = (uint8_t)(len & 0xFFU);
	sha512_compress(hash, block);

	// Change endianness of the internal state to big-endian
	hash[0] = __builtin_bswap64(hash[0]);
	hash[1] = __builtin_bswap64(hash[1]);
	hash[2] = __builtin_bswap64(hash[2]);
	hash[3] = __builtin_bswap64(hash[3]);
	hash[4] = __builtin_bswap64(hash[4]);
	hash[5] = __builtin_bswap64(hash[5]);
	hash[6] = __builtin_bswap64(hash[6]);
	hash[7] = __builtin_bswap64(hash[7]);
}