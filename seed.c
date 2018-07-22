#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "seed.h"

void next_seed(uint64_t *seed) {
    uint64_t carry = 0;
    seed[0] = seed[0] == __UINT64_MAX__ ? carry++ : seed[0] + 1;
    if (carry) seed[1] = seed[1] == __UINT64_MAX__ ? 0 : seed[1] + carry--;
    if (carry) seed[2] = seed[2] == __UINT64_MAX__ ? 0 : seed[2] + carry--;
    if (carry) seed[3] = seed[3] == __UINT64_MAX__ ? 0 : seed[3] + 1;
}

void init_seed(uint64_t *seed) {
    seed[3] = time(NULL);
    seed[2] = ((uint64_t)getpid()) << 32;
    seed[1] = 0;
    seed[0] = 0;
}
