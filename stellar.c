#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crc16/crc16.h"
#include "base32/base32.h"
#include "stellar.h"


inline void encode_check(unsigned char dest[56], const uint8_t src[32], const uint8_t vbyte)
{
    uint8_t payload[35];
    memcpy(payload + 1, src, 32);
    payload[0] = vbyte;

    const uint16_t crc = crc16xmodem(payload, 33);
    *((uint16_t *)(payload + 33)) = crc;

    base32_encode(payload, 35, dest, 56);
}


void stellar_address(unsigned char address[56], const uint8_t public_key[32])
{
    encode_check(address, public_key, 0x30);
}


void stellar_secret(unsigned char secret[56], const uint8_t seed[32])
{
    encode_check(secret, seed, 0x90);
}
