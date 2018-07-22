#ifndef __BLOOMLIB_H_
#define __BLOOMLIB_H_

//
// Copyright Stanislav Seletskiy <s.seletskiy@gmail.com>
// Modified by: hmatejx, 2018
//

#include <stdint.h>

#define BF_LOG2   0.6931471805599453
#define BF_LOG2_2 0.4804530139182013

#define BF_KEY_BUFFER_SIZE 255
#define BF_DUMP_SIGNATURE 0xb100f11e

typedef unsigned char    bf_bitmap_t;
typedef uint64_t         bf_index_t;
typedef unsigned char    bf_hash_t;

typedef struct bf_s      bf_t;

struct bf_s
{
	bf_index_t bits_count;
	bf_index_t bits_used;
	bf_index_t bytes_count;

	double error_rate;

	bf_bitmap_t* bitmap;

	bf_index_t (*hash_func)(char*, unsigned int);
	unsigned int hashes_count;
};

bf_t* bf_create(double error, bf_index_t key_count);
void  bf_destroy(bf_t* filter);

void  bf_add(bf_t* filter, const char* const key, const unsigned int len);
unsigned int  bf_has(bf_t* filter, const char* const key, const unsigned int len);

#endif