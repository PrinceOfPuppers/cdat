#ifndef CDAT_HASH_SET_H_
#define CDAT_HASH_SET_H_

#include <stdlib.h>
#include <cdat/linked-list.h>
#include <cdat/helpers.h>

typedef struct Hash_Set {
    // to be iterable
    Linked_List keys_ll;

    Linked_List *hs_arr;
    size_t hash_max;

    int copy_on_write;
} Hash_Set;

Hash_Set *hs_create(size_t hash_max, cdat_cmp_func map, int copy_on_write);


int hs_is_in(Hash_Set *hs, void *val, size_t val_size);


int hs_add(Hash_Set *hs, void *val, size_t val_size);

void hs_from_ll(Hash_Set *hs, Linked_List *ll);


int hs_try_remove(Hash_Set *hs, void *val, size_t val_size);

Linked_List_Node *hs_next(Hash_Set *hs);

void hs_iter_reset(Hash_Set *hs);

size_t hs_len(Hash_Set *hs);

// result gets cmp function from hs1
Hash_Set *hs_union(Hash_Set *hs1, Hash_Set *hs2, int copy_on_write);

// result gets cmp function from hs1
Hash_Set *hs_intersection(Hash_Set *hs1, Hash_Set *hs2, int copy_on_write);

int hs_is_subset(Hash_Set *potential_subset, Hash_Set *potential_superset);

int hs_are_equal(Hash_Set *hs1, Hash_Set *hs2);

void hs_map( Hash_Set *hs, cdat_map_func map );

void hs_test();
#endif
