#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "hashset.h"

#define CHECK_NOT_NULL(x) if (NULL == (x)) { return ERR_PARAMS; }
#define CHECK_NULL(x)     if (NULL != (x)) { return ERR_PARAMS; }
#define CHECK_POSITIVE(x) if (0 >= (x))    { return ERR_PARAMS; }
#define CHECK_MALLOC(x)   if (NULL == (x)) { return ERR_MEM; }

//// IMPLEMENTATION
/* IDEA - keep elements in a managed heap, and keep relative addresses in
 * the hash buckets.
 */
// TODO deserialize with objcopy

struct set_t {
  uint32_t size;     // maximum number of elements, number of buckets
  uint32_t capacity; // maximum number of bytes
  hasher_t hasher; // hashing function
  uint32_t *map;     // array of pointers to elements
  void *heap;      // pointer to the heap of elements
  void *tail;      // pointer to the start of free space in the heap
};

int set_alloc(set **ptr, uint32_t size, uint32_t capacity, hasher_t func) {
  if (NULL == ptr  ||
      NULL != *ptr ||
      NULL == func ||
      0 >= size ||
      0 >= capacity) return ERR_PARAMS;

  // [struct | map | heap]
  uint32_t total = sizeof(set) + size * sizeof(uint32_t) + capacity;
  void *mem = calloc(1, total);
  CHECK_MALLOC(mem);

  set *candidate      = mem;

  candidate->size     = size;
  candidate->capacity = capacity;
  candidate->hasher   = func;
  candidate->map      = (void *)((char *)mem + sizeof(set));
  candidate->heap     = (void *)((char *)mem + sizeof(set) + size * sizeof(uint32_t));
  candidate->tail     = (void *)((char *)candidate->heap + 1);

  *ptr = candidate;
  return SUCCESS;
}

int set_free(set **ptr) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(*ptr);

  free(*ptr);
  *ptr = NULL;
  return SUCCESS;
}

// TODO use masking instead of mod for power of 2
// TODO try double hashing
static inline int find_index(uint32_t hash, unsigned int probe, uint32_t size) {
  const float i = 0.5 * probe;
  return (hash + (int)(i + 2.0*i*i)) % size;
}

static inline bool has_space(set *ptr, uint32_t len) {
  uint32_t room = ptr->capacity - ((char *)ptr->tail - (char *)ptr->heap);
  return room > len;
}

int set_add(set *ptr, const void *element, uint32_t len) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(element);
  CHECK_POSITIVE(len);

  if (!has_space(ptr, len)) return ERR_MEM;

  uint32_t hash = ptr->hasher(element, len);
  for (unsigned int i=0; i < ptr->size; i++) {
    uint32_t index = find_index(hash, i, ptr->size);
    if (!ptr->map[index]) { // insert
      ptr->map[index] = (char *)ptr->tail - (char *)ptr->heap;
      memcpy(ptr->tail, element, len);
      ptr->tail = (void *)((char *)ptr->tail + len);
      return SUCCESS;
    }
  }

  return ERR_SIZE;
}

int set_find(const set *ptr, const void *needle, uint32_t len) {
  /*
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(needle);
  CHECK_POSITIVE(len);
  */

  uint32_t hash = ptr->hasher(needle, len);
  for (unsigned int i=0; i < ptr->size; i++) {
    uint32_t index = find_index(hash, i, ptr->size);
    if (!ptr->map[index]) return ERR_NOT_FOUND;
    // get offset, add to heap
    const void *candidate = (void *)((char *)ptr->heap + ptr->map[index]);
    if (!memcmp(needle, candidate, len)) return SUCCESS;
  }

  return ERR_NOT_FOUND;
}

int set_dump(const set *ptr, FILE *file) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(file);

  uint32_t total   = sizeof(set) + ptr->size * sizeof(uint32_t) + ptr->capacity;
  uint32_t written = fwrite(ptr, total, 1, file);
  if (written < 1) return ERR_IO;
  return SUCCESS;
}

int set_load(set **ptr, void *mem, hasher_t func) {
  CHECK_NOT_NULL(ptr);
  CHECK_NOT_NULL(mem);
  CHECK_NULL(*ptr);
  CHECK_NOT_NULL(func);

  set *candidate = mem;
  uint32_t size    = candidate->size;
  candidate->hasher = func;
  candidate->map    = (void *)((char *)mem + sizeof(set));
  candidate->heap   = (void *)((char *)mem + sizeof(set) + size * sizeof(uint32_t));
  candidate->tail   = (void *)((char *)candidate->heap + 1);

  *ptr = candidate;
  return SUCCESS;
}
