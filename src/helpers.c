#include <cdat/helpers.h>

#include <stdio.h>
#include <assert.h>

uint32_t hash_digit_fold(void *key, size_t key_size, uint32_t hash_max){
    assert(key_size < MAX_HASH_DIGIT_FOLD_KEY_LEN);

    uint32_t hash = 0;

    for(int i = 0; i < key_size; i++){
        hash += *(uint8_t *)(key+i*sizeof(uint8_t));
    }

    return hash % hash_max;
}

void *malloc_wrapper(size_t size){
    void *p = malloc(size);
    if (!p){
        perror("Memory Allocation Failure");
        exit(1);
    }
    return p;
}
