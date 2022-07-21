#include <cdat/helpers.h>

#include <stdio.h>

void *malloc_wrapper(size_t size){
    void *p = malloc(size);
    if (!p){
        perror("Memory Allocation Failure");
        exit(1);
    }
    return p;
}
