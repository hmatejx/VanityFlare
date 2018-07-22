//
// Copyright Stanislav Seletskiy <s.seletskiy@gmail.com>
// Modified by: hmatejx, 2018
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

#include "bloomlib.h"
#include "city.h"

#define FALSE 0
#define TRUE  1

typedef unsigned int     uint;

unsigned int bf_bitmap_set(bf_bitmap_t* bitmap, bf_index_t bit);
unsigned int bf_bitmap_get(bf_bitmap_t* bitmap, bf_index_t bit);
//bf_index_t murmur(char* const key, const uint key_len);
bf_index_t bf_get_index(bf_t* filter, const char* key, unsigned int key_len, unsigned int hash_index);


bf_t* bf_create(double error, bf_index_t est_key_count)
{
	bf_t* filter = (bf_t*)malloc(sizeof(bf_t));
	assert(filter != NULL);

	filter->bits_count = (bf_index_t)ceil(- (double)est_key_count * log(error) / BF_LOG2_2);

	filter->bytes_count = (bf_index_t)ceil(filter->bits_count / 8.);
	filter->bitmap = (bf_bitmap_t*)malloc(sizeof(bf_bitmap_t) * filter->bytes_count);
	memset(filter->bitmap, 0, filter->bytes_count);
	filter->hash_func = CityHash64; //murmur;
	filter->hashes_count = (bf_index_t)ceil(filter->bits_count / est_key_count * BF_LOG2);
	filter->error_rate = error;
	filter->bits_used = 0;

	assert(filter->bitmap != NULL);
	assert(filter->hash_func != NULL);

	return filter;
}


bf_index_t bf_get_index(bf_t* filter, const char* key, uint key_len, uint hash_index)
{
	char buffer[BF_KEY_BUFFER_SIZE] = { 0 };

	memcpy(buffer, key, key_len);
	sprintf(buffer + key_len, "%03d", hash_index);

	bf_index_t index = filter->hash_func(buffer, key_len + 3);

	return index % filter->bits_count;
}


void bf_add(bf_t* filter, const char* const key, const uint key_len)
{
	unsigned int hash_index = 0;

	assert(key_len + filter->hashes_count / 10 + 1 < BF_KEY_BUFFER_SIZE);

	if (key_len == 0) {
		return;
	}

	for (hash_index = 0; hash_index < filter->hashes_count; hash_index++) {
		uint changed = bf_bitmap_set(filter->bitmap,
			bf_get_index(filter, key, key_len, hash_index));

		filter->bits_used += changed;
	}
}


uint bf_has(bf_t* filter, const char* const key, const uint key_len)
{
	uint result = TRUE;
	uint hash_index = 0;

	assert(key_len + filter->hashes_count / 10 + 1 < BF_KEY_BUFFER_SIZE);

	for (hash_index = 0; hash_index < filter->hashes_count; hash_index++) {
		result = result && bf_bitmap_get(filter->bitmap,
			bf_get_index(filter, key, key_len, hash_index));
	}

	return result;
}


uint bf_bitmap_set(bf_bitmap_t* bitmap, bf_index_t bit)
{
	uint cell_size = sizeof(bf_bitmap_t) * 8;

	bf_index_t byte = bit / cell_size;
	char bit_pos = bit - byte * cell_size;

	char old_byte = bitmap[byte];
	bitmap[byte] |= (1 << (cell_size - bit_pos - 1));

	return bitmap[byte] != old_byte;
}


uint bf_bitmap_get(bf_bitmap_t* bitmap, bf_index_t bit)
{
	uint cell_size = sizeof(bf_bitmap_t) * 8;

	bf_index_t byte = bit / cell_size;
	char bit_pos = bit - byte * cell_size;

	return bitmap[byte] & (1 << (cell_size - bit_pos - 1));
}


void bf_destroy(bf_t* filter)
{
	assert(filter != NULL);
	assert(filter->bitmap != NULL);

	free(filter->bitmap);
	free(filter);
}


/*
bf_index_t murmur(char* const key, const uint key_len)
{
	uint seed = 1;
	const uint64_t m = 0xc6a4a7935bd1e995LLU;
	const int r = 47;

	uint64_t h = seed ^ (key_len * m);

	const uint64_t * data = (const uint64_t *)key;
	const uint64_t * end = data + (key_len / 8);

	while(data != end)
	{
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch(key_len & 7)
	{
	case 7: h ^= (uint64_t)data2[6] << 48;
	case 6: h ^= (uint64_t)data2[5] << 40;
	case 5: h ^= (uint64_t)data2[4] << 32;
	case 4: h ^= (uint64_t)data2[3] << 24;
	case 3: h ^= (uint64_t)data2[2] << 16;
	case 2: h ^= (uint64_t)data2[1] << 8;
	case 1: h ^= (uint64_t)data2[0];
			h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}
*/