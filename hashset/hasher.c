#include "hasher.h"

inline uint32_t hasher(const void *element, uint32_t len) {
  register uint32_t code = 0;
  register const char *buffer = element;
  register uint32_t i = 0;

  for (i = 0; i < len; i++) {
    code  = buffer[i] + code * 31;
  }

  return code;
}
