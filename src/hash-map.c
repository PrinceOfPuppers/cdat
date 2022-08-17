#include <cdat/hash-map.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <cdat/helpers.h>

int hm_ll_data_cmp(void *va, size_t va_size, void *vb, size_t vb_size){
    assert(va_size == sizeof(Hash_Map_LL_Node_Data));
    assert(vb_size == sizeof(Hash_Map_LL_Node_Data));
    Hash_Map_LL_Node_Data *da = (Hash_Map_LL_Node_Data *)va;
    Hash_Map_LL_Node_Data *db = (Hash_Map_LL_Node_Data *)vb;

    if (da->key_size != db->key_size){
        return 0;
    }
    return memcmp(da->key, db->key, da->key_size);
}
int hm_key_cmp(void *va, size_t va_size, void *vb, size_t vb_size){
    if (va_size != vb_size){
        return 0;
    }
    return memcmp(va, vb, va_size);
}

Hash_Map *hm_create(size_t hash_max, int copy_on_write){
    Hash_Map *hm = malloc_wrapper(sizeof(Hash_Map));
    hm->hash_max = hash_max;
    hm->copy_on_write = copy_on_write;
    ll_init_empty(&hm->keys_ll, hm_key_cmp, 0);

    // init array of ll
    hm->hm_arr = malloc_wrapper(sizeof(Linked_List)*hash_max);

    // init all ll
    for(int i = 0; i < hash_max; i++){
        ll_init_empty(&(hm->hm_arr)[i], hm_ll_data_cmp, 0);
    }

    return hm;
}


void hm_pop(Hash_Map *hm, void *out_key, size_t *out_key_size, void *out_val, size_t *out_val_size){
    assert(hm->keys_ll.len > 0);

    Hash_Map_LL_Node_Data key_wrapper;

    key_wrapper.val = NULL;
    key_wrapper.val_size = 0;

    key_wrapper.key = ll_pop_front(&hm->keys_ll, &key_wrapper.key_size);

    uint32_t h = hash_digit_fold(key_wrapper.key, key_wrapper.key_size, hm->hash_max);

    Hash_Map_LL_Node_Data *container = (Hash_Map_LL_Node_Data *)ll_try_pop_val(&hm->hm_arr[h], (void *)&key_wrapper, sizeof(Hash_Map_LL_Node_Data), NULL);
    assert(container != NULL);

    out_key = container->key;
    out_val = container->val;

    if(out_key_size != NULL){
        *out_key_size = container->key_size;
    }
    if(out_val_size != NULL){
        *out_val_size = container->val_size;
    }

    free(container);
}

// destroys hm and its linked lists, does not touch values
void hm_free_keep_keys_and_vals(Hash_Map *hm){
    while(hm->keys_ll.len > 0){
        hm_pop(hm, NULL, NULL, NULL, NULL);
    }

    free(hm);
}

void hm_free_free_keys_and_vals(Hash_Map *hm){
    void *key = NULL;
    void *val = NULL;
    while(hm->keys_ll.len > 0){
        hm_pop(hm, key, NULL, val, NULL);
        free(key);
        free(val);
    }
    free(hm);
}

void hm_free(Hash_Map *hm){
    if(hm->copy_on_write){
        return hm_free_free_keys_and_vals(hm);
    }
    return hm_free_keep_keys_and_vals(hm);
}


// also returns hash
static int _hm_is_in(Hash_Map *hm, void *key, size_t key_size, uint32_t *h){
    *h = hash_digit_fold(key, key_size, hm->hash_max);
    if (hm->hm_arr[*h].len == 0){
        return 0;
    }

    Hash_Map_LL_Node_Data key_wrapper;
    key_wrapper.val = NULL;
    key_wrapper.val_size = 0;
    key_wrapper.key = key;
    key_wrapper.key_size = key_size;
    return ll_is_in(&hm->hm_arr[*h], &key_wrapper, sizeof(Hash_Map_LL_Node_Data));
}

int hm_is_in(Hash_Map *hm, void *key, size_t key_size){
    uint32_t h;
    return _hm_is_in(hm, key, key_size, &h);
}

int hm_try_get(Hash_Map *hm, void *key, size_t key_size, void *out_val, size_t *out_val_size){
    uint32_t h = hash_digit_fold(key, key_size, hm->hash_max);
    if (hm->hm_arr[h].len == 0){
        return 0;
    }

    Hash_Map_LL_Node_Data key_wrapper;
    key_wrapper.val = NULL;
    key_wrapper.val_size = 0;
    key_wrapper.key = key;
    key_wrapper.key_size = key_size;

    Hash_Map_LL_Node_Data *container = ll_try_get_val(&hm->hm_arr[h], &key_wrapper, sizeof(Hash_Map_LL_Node_Data), NULL);
    if(container == NULL){
        return 0;
    }

    out_val = container->val;
    if(out_val_size != NULL){
        *out_val_size = container->val_size;
    }
    return 1;
}
void hm_get(Hash_Map *hm, void *key, size_t key_size, void *out_val, size_t *out_val_size){
    assert(hm_try_get(hm, key, key_size, out_val, out_val_size));
}



// returns 1 if added, 0 if already exists
static int _hm_add_ref(Hash_Map *hm, void *key, size_t key_size, void *val, size_t val_size){
    uint32_t h;
    if(!_hm_is_in(hm, key, key_size, &h)){
        Hash_Map_LL_Node_Data* container = malloc_wrapper(sizeof(Hash_Map_LL_Node_Data));
        container->key = key;
        container->key_size = key_size;
        container->val = val;
        container->val_size = val_size;
        ll_append(&hm->hm_arr[h], container, sizeof(Hash_Map_LL_Node_Data));
        ll_append(&hm->keys_ll, key, key_size);
        return 1;
    }
    return 0;
}

static int _hm_add_copy(Hash_Map *hm, void *key, size_t key_size, void *val, size_t val_size){
    uint32_t h;
    if(!_hm_is_in(hm, key, key_size, &h)){
        void *k = malloc_wrapper(key_size);
        void *v = malloc_wrapper(val_size);
        memcpy(k, key, key_size);
        memcpy(v, val, val_size);

        Hash_Map_LL_Node_Data* container = malloc_wrapper(sizeof(Hash_Map_LL_Node_Data));
        container->key = k;
        container->key_size = key_size;
        container->val = v;
        container->val_size = val_size;
        ll_append(&hm->hm_arr[h], container, sizeof(Hash_Map_LL_Node_Data));
        ll_append(&hm->keys_ll, k, key_size);
        return 1;
    }
    return 0;
}

int hm_add(Hash_Map *hm, void *key, size_t key_size, void *val, size_t val_size){
    if(hm->copy_on_write){
        return _hm_add_copy(hm, key, key_size, val, val_size);
    }
    return _hm_add_ref(hm, key, key_size, val, val_size);
}

int hm_try_pop_val(Hash_Map *hm, void *key, size_t key_size, void *key_out, size_t key_out_size, void *val_out, size_t *val_out_size){
    uint32_t h = hash_digit_fold(key, key_size, hm->hash_max);
    if (hm->hm_arr[h].len == 0){
        return 0;
    }

    Hash_Map_LL_Node_Data key_wrapper;
    key_wrapper.val = NULL;
    key_wrapper.val_size = 0;
    key_wrapper.key = key;
    key_wrapper.key_size = key_size;

    val_out = ll_try_pop_val(&hm->hm_arr[h], &key_wrapper, sizeof(Hash_Map_LL_Node_Data), val_out_size);

    if(val_out != NULL){
        assert(ll_try_pop_val(&hm->keys_ll, key, key_size, NULL) != NULL);
        return 1;
    }
    return 0;
}

Linked_List_Node *hm_next(Hash_Map *hm){
    return ll_next(&hm->keys_ll);
}

void hm_iter_reset(Hash_Map *hm){
    ll_iter_reset(&hm->keys_ll);
}

size_t hm_len(Hash_Map *hm){
    return hm->keys_ll.len;
}

Hash_Map *hm_key_union(Hash_Map *hm1, Hash_Map *hm2, int copy_on_write){
    Hash_Map *hm = hm_create(
            hm1->hash_max > hm2->hash_max ? hm1->hash_max : hm2->hash_max, 
            copy_on_write
    );
    void *val = NULL;
    size_t val_size;
    Linked_List_Node *key_node = hm1->keys_ll.head;
    while(key_node != NULL){
        hm_get(hm1, key_node->val, key_node->val_size, val, &val_size);
        hm_add(hm, key_node->val, key_node->val_size, val, val_size);
        key_node = key_node->next;
    }

    key_node = hm2->keys_ll.head;
    while(key_node != NULL){
        hm_get(hm2, key_node->val, key_node->val_size, val, &val_size);
        hm_add(hm, key_node->val, key_node->val_size, val, val_size);
        key_node = key_node->next;
    }
    return hm;
}

Hash_Map *hm_key_intersection(Hash_Map *hm1, Hash_Map *hm2, int copy_on_write){
    Hash_Map *hm = hm_create(
            hm1->hash_max > hm2->hash_max ? hm1->hash_max : hm2->hash_max, 
            copy_on_write
    );
    void *val = NULL;
    size_t val_size;
    Linked_List_Node *key_node = hm1->keys_ll.head;
    while(key_node != NULL){
        if(hm_is_in(hm2, key_node->val, key_node->val_size)){
            hm_get(hm1, key_node->val, key_node->val_size, val, &val_size);
            hm_add(hm, key_node->val, key_node->val_size, val, val_size);
        }

        key_node = key_node->next;
    }

    return hm;
}

int hm_is_key_subset(Hash_Map *potential_subset, Hash_Map *potential_superset){
    Linked_List_Node *n = potential_subset->keys_ll.head;
    while(n != NULL){
        if(!hm_is_in(potential_superset, n->val, n->val_size)){
            return 0;
        }
        n = n->next;
    }

    return 1;
}

int hm_is_key_equal(Hash_Map *hm1, Hash_Map *hm2){
    if(hm_len(hm1) != hm_len(hm2)){
        return 0;
    }

    return hm_is_key_subset(hm1, hm2);
}

// TODO: undealtwith
void hm_map(Hash_Map *hm, cdat_map_func map){
    ll_map(&hm->keys_ll, map);
}

// TODO: undealtwith
int _hm_test_cmp(void *va, size_t va_size, void *vb, size_t vb_size){
    if(va_size != vb_size){
        return 0;
    }

    return memcmp(va, vb, va_size) == 0;
}

// TODO: undealtwith
void hm_test(){
    puts("hm_test: creation, no copy on write");

    cdat_cmp_func cmp = *_hm_test_cmp;

    Hash_Map *ha = hm_create(10000, cmp, 0);


    puts("hm_test: len, no copy on write");
    assert(hm_len(ha) == 0);
    

    puts("hm_test: add, no copy on write");
    hm_add(ha, "test", 4);
    hm_add(ha, "test", 4);
    assert(hm_len(ha) == 1);


    puts("hm_test: is in, no copy on write");
    assert(hm_is_in(ha, "test", 4));
    assert(!hm_is_in(ha, "testing", 5));


    puts("hm_test: remove, no copy on write");
    hm_try_remove(ha, "test", 4);
    assert(!hm_is_in(ha, "test", 4));
    assert(hm_len(ha) == 0);


    puts("hm_test: creation, copy on write");
    Hash_Map *hb = hm_create(300, cmp, 1);


    puts("hm_test: add, copy on write");
    hm_add(hb, "test", 4);
    hm_add(hb, "test", 4);
    assert(hm_len(hb) == 1);
    

    puts("hm_test: is in, copy on write");
    assert(hm_is_in(hb, "test", 4));
    assert(!hm_is_in(hb, "testing", 7));


    puts("hm_test: is subset");
    hm_add(ha, "test", 4);
    hm_add(hb, "testing", 7);

    assert(hm_is_subset(ha, hb));
    assert(!hm_is_subset(hb, ha));


    puts("hm_test: intersection");
    Hash_Map *hc = hm_intersection(ha, hb, 1);
    assert(hm_len(hc) == 1);
    assert(hm_is_in(hc, "test", 4));

    puts("hm_test: union");
    Hash_Map *hd = hm_union(ha, hb, 1);
    assert(hm_len(hd) == 2);
    assert(hm_is_in(hd, "test", 4));
    assert(hm_is_in(hd, "testing", 7));


    puts("hm_test: pop, copy_on_write");
    size_t val_size;
    void *val = hm_pop(hd, &val_size);
    assert(cmp(val, val_size, "test", 4) || cmp(val, val_size, "testing", 7));
    free(val);

    puts("hm_test: free");
    hm_free(ha);
    hm_free(hb);
    hm_free(hc);
    hm_free(hd);

    puts("hm_test: done");
}

