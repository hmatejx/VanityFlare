#ifndef __SHA256_H_
#define __SHA256_H_

#include <stdint.h>

void next_seed(uint64_t *seed);
void init_seed(uint64_t *seed);

#endif