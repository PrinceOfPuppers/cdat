#ifndef CDAT_HELPERS_H_
#define CDAT_HELPERS_H_

#include <stdlib.h>
#include <stdint.h>

#define MAX_HASH_DIGIT_FOLD_KEY_LEN 16e7
uint32_t hash_digit_fold(void *key, size_t key_size, uint32_t hash_max);

void *malloc_wrapper(size_t size);

#endif
