#ifndef __STELLAR_H_
#define __STELLAR_H_

void stellar_address(unsigned char address[56], const uint8_t public_key[32]);
void stellar_secret(unsigned char address[56], const uint8_t public_key[32]);

# endif