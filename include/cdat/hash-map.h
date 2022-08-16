#ifndef CDAT_HASH_MAP_H_
#define CDAT_HASH_MAP_H_

#include <stdlib.h>
#include <cdat/linked-list.h>
#include <cdat/helpers.h>

typedef struct Hash_Map {
    // to be iterable
    Linked_List keys_ll;

    Linked_List *hm_arr;
    size_t hash_max;

    int copy_on_write;
} Hash_Map;

typedef struct Hash_Map_LL_Data{
    void *key;
    size_t key_size;
    void *val;
    size_t val_size;
}Hash_Map_LL_Node_Data;

// used in hash map data ll to compare Hash_Map_LL_Data to see if there is a match
int hm_ll_data_cmp(void *va, size_t va_size, void *vb, size_t vb_size);

Hash_Map *hm_create(size_t hash_max, int copy_on_write);

void hm_pop(Hash_Map *hm, void *out_key, size_t *out_key_size, void *out_val, size_t *out_val_size);

// destroys hm and its linked lists, does not touch values
void hm_free_keep_keys_and_val(Hash_Map *hm);
void hm_free_free_keys_and_vals(Hash_Map *hm);
void hm_free(Hash_Map *hm);

int hm_is_in(Hash_Map *hm, void *key, size_t key_size);

int hm_add(Hash_Map *hm, void *key, size_t key_size, void *val, size_t val_size);


int hm_try_pop_val(Hash_Map *hm, void *key, size_t key_size, void *key_out, size_t key_out_size, void *val_out, size_t *val_out_size);

Linked_List_Node *hm_next(Hash_Map *hm);
void hm_iter_reset(Hash_Map *hm);

size_t hm_len(Hash_Map *hm);

// result gets cmp function from hm1
Hash_Map *hm_union(Hash_Map *hm1, Hash_Map *hm2, int copy_on_write);

// result gets cmp function from hm1
Hash_Map *hm_intersection(Hash_Map *hm1, Hash_Map *hm2, int copy_on_write);

int hm_is_subset(Hash_Map *potential_subset, Hash_Map *potential_superset);

int hm_are_equal(Hash_Map *hm1, Hash_Map *hm2);

void hm_map( Hash_Map *hm, cdat_map_func map );

void hm_test();
#endif
