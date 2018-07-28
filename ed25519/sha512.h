#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>
#include <stddef.h>

void crypto_hash_sha512(unsigned char* const hash, const unsigned char* message, size_t len);

#endif
