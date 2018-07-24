#include <string.h>
#include "sha512.h"
#include "ge25519.h"

int ed25519_create_keypair(unsigned char *pk, unsigned char *sk, unsigned char* seed)
{
  unsigned char az[64];
  sc25519 scsk;
  ge25519 gepk;

  memcpy(sk, seed, 32);
  crypto_hash_sha512(az,sk,32);
  az[0] &= 248;
  az[31] &= 127;
  az[31] |= 64;

  sc25519_from32bytes(&scsk,az);

  ge25519_scalarmult_base(&gepk, &scsk);
  ge25519_pack(pk, &gepk);
  memmove(sk + 32,pk,32);
  return 0;
}
