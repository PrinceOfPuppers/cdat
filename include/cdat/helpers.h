#ifndef CDAT_HELPERS_H_
#define CDAT_HELPERS_H_

#include <stdlib.h>
#include <stdint.h>

typedef int (*cdat_cmp_func)(void *va, size_t va_size, void *vb, size_t vb_size);
typedef void (*cdat_map_func)(int index, void *val, size_t val_size);

#define MAX_HASH_DIGIT_FOLD_KEY_LEN 16e7
uint32_t hash_digit_fold(void *key, size_t key_size, uint32_t hash_max);

void *malloc_wrapper(size_t size);

#endif
