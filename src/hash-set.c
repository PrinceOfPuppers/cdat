#include <cdat/hash-set.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <cdat/helpers.h>

Hash_Set *hs_create(size_t hash_max, cdat_cmp_func cmp, int copy_on_write){
    Hash_Set *hs = malloc_wrapper(sizeof(Hash_Set));
    hs->hash_max = hash_max;
    hs->copy_on_write = copy_on_write;
    ll_init_empty(&hs->keys_ll, cmp, 0);

    // init array of ll
    hs->hs_arr = malloc_wrapper(sizeof(Linked_List)*hash_max);

    // init all ll
    for(int i = 0; i < hash_max; i++){
        ll_init_empty(&(hs->hs_arr)[i], cmp, 0);
    }

    return hs;
}

void *hs_pop(Hash_Set *hs, size_t *out_size){
    size_t val_size;
    void *val;
    uint32_t h;
    assert(hs->keys_ll.len > 0);
    val = ll_pop_front(&hs->keys_ll, &val_size);
    h = hash_digit_fold(val, val_size, hs->hash_max);

    // x and val should point to same memory
    void *x = ll_try_pop_val(&hs->hs_arr[h], val, val_size, NULL);
    assert(x!=NULL);
    //assert(ll_try_remove_val(&hs->hs_arr[h], val, val_size));
    if(out_size != NULL){
        *out_size = val_size;
    }
    return val;
}

// destroys hs and its linked lists, does not touch values
void hs_free_keep_vals(Hash_Set *hs){
    while(hs->keys_ll.len > 0){
        hs_pop(hs, NULL);
    }

    free(hs);
}

void hs_free_free_vals(Hash_Set *hs){
    while(hs->keys_ll.len > 0){
        free(hs_pop(hs, NULL));
    }
    free(hs);
}

void hs_free(Hash_Set *hs){
    if(hs->copy_on_write){
        return hs_free_free_vals(hs);
    }
    return hs_free_keep_vals(hs);
}


int hs_is_in(Hash_Set *hs, void *val, size_t val_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    return ll_is_in(&hs->hs_arr[h], val, val_size);
}

// returns 1 if added, 0 if already exists
static int _hs_add_ref(Hash_Set *hs, void *val, size_t val_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    if(!ll_is_in(&hs->hs_arr[h], val, val_size)){
        ll_append(&hs->hs_arr[h], val, val_size);
        ll_append(&hs->keys_ll, val, val_size);
        return 1;
    }
    return 0;
}

static int _hs_add_copy(Hash_Set *hs, void *val, size_t val_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    if(!ll_is_in(&hs->hs_arr[h], val, val_size)){
        void *v = malloc_wrapper(val_size);
        memcpy(v, val, val_size);
        ll_append(&hs->hs_arr[h], v, val_size);
        ll_append(&hs->keys_ll, v, val_size);
        return 1;
    }
    return 0;
}

int hs_add(Hash_Set *hs, void *val, size_t val_size){
    if(hs->copy_on_write){
        return _hs_add_copy(hs, val, val_size);
    }
    return _hs_add_ref(hs, val, val_size);
}

void hs_from_ll(Hash_Set *hs, Linked_List *ll){
    //Hash_Set *hs = hs_create(hash_max);
    Linked_List_Node *n = ll->head;

    while(n != NULL){
        hs_add(hs, n->val, n->val_size);
        n = n->next;
    }
}

void hs_from_array(Hash_Set *hs, void *arr, size_t arr_len, size_t val_size){
    for(int i = 0; i < arr_len; i++){
        hs_add(hs, arr + i*val_size, val_size);
    }
}


void *hs_try_pop_val(Hash_Set *hs, void *val, size_t val_size, size_t *out_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    void *res = ll_try_pop_val(&hs->hs_arr[h], val, val_size, out_size);
    if(res != NULL){
        assert(ll_try_pop_val(&hs->keys_ll, val, val_size, out_size) != NULL);
        return res;
    }
    return NULL;
}
int hs_try_free_val(Hash_Set *hs, void *val, size_t val_size){
    void *x = hs_try_pop_val(hs, val, val_size, NULL);
    if(x != NULL){
        free(x);
        return 1;
    }
    return 0;
}

Linked_List_Node *hs_next(Hash_Set *hs){
    return ll_next(&hs->keys_ll);
}

void hs_iter_reset(Hash_Set *hs){
    ll_iter_reset(&hs->keys_ll);
}

size_t hs_len(Hash_Set *hs){
    return hs->keys_ll.len;
}

Hash_Set *hs_union(Hash_Set *hs1, Hash_Set *hs2, int copy_on_write){
    Hash_Set *h = hs_create(
            hs1->hash_max > hs2->hash_max ? hs1->hash_max : hs2->hash_max, 
            hs1->keys_ll.cmp,
            copy_on_write
    );
    hs_from_ll(h, &hs1->keys_ll);
    hs_from_ll(h, &hs2->keys_ll);
    return h;
}

Hash_Set *hs_intersection(Hash_Set *hs1, Hash_Set *hs2, int copy_on_write){
    Hash_Set *h = hs_create(
            hs1->hash_max > hs2->hash_max ? hs1->hash_max : hs2->hash_max, 
            hs1->keys_ll.cmp,
            copy_on_write
    );

    Linked_List_Node *n = hs1->keys_ll.head;
    while(n != NULL){
        if(hs_is_in(hs2, n->val, n->val_size)){
            hs_add(h, n->val, n->val_size);
        }
        n = n->next;
    }

    return h;
}

int hs_is_subset(Hash_Set *potential_subset, Hash_Set *potential_superset){
    Linked_List_Node *n = potential_subset->keys_ll.head;
    while(n != NULL){
        if(!hs_is_in(potential_superset, n->val, n->val_size)){
            return 0;
        }
        n = n->next;
    }

    return 1;
}

int hs_are_equal(Hash_Set *hs1, Hash_Set *hs2){
    if(hs_len(hs1) != hs_len(hs2)){
        return 0;
    }

    return hs_is_subset(hs1, hs2);
}

void hs_map(Hash_Set *hs, cdat_map_func map){
    ll_map(&hs->keys_ll, map);
}

int _hs_test_cmp(void *va, size_t va_size, void *vb, size_t vb_size){
    if(va_size != vb_size){
        return 0;
    }

    return memcmp(va, vb, va_size) == 0;
}

void hs_test(){
    puts("hs_test: creation, no copy on write");

    cdat_cmp_func cmp = *_hs_test_cmp;

    Hash_Set *ha = hs_create(10000, cmp, 0);


    puts("hs_test: len, no copy on write");
    assert(hs_len(ha) == 0);
    

    puts("hs_test: add, no copy on write");
    hs_add(ha, "test", 4);
    hs_add(ha, "test", 4);
    assert(hs_len(ha) == 1);


    puts("hs_test: is in, no copy on write");
    assert(hs_is_in(ha, "test", 4));
    assert(!hs_is_in(ha, "testing", 5));


    puts("hs_test: remove, no copy on write");
    hs_try_pop_val(ha, "test", 4, NULL);
    assert(!hs_is_in(ha, "test", 4));
    assert(hs_len(ha) == 0);


    puts("hs_test: creation, copy on write");
    Hash_Set *hb = hs_create(300, cmp, 1);


    puts("hs_test: add, copy on write");
    hs_add(hb, "test", 4);
    hs_add(hb, "test", 4);
    assert(hs_len(hb) == 1);
    

    puts("hs_test: is in, copy on write");
    assert(hs_is_in(hb, "test", 4));
    assert(!hs_is_in(hb, "testing", 7));


    puts("hs_test: is subset");
    hs_add(ha, "test", 4);
    hs_add(hb, "testing", 7);

    assert(hs_is_subset(ha, hb));
    assert(!hs_is_subset(hb, ha));


    puts("hs_test: intersection");
    Hash_Set *hc = hs_intersection(ha, hb, 1);
    assert(hs_len(hc) == 1);
    assert(hs_is_in(hc, "test", 4));

    puts("hs_test: union");
    Hash_Set *hd = hs_union(ha, hb, 1);
    assert(hs_len(hd) == 2);
    assert(hs_is_in(hd, "test", 4));
    assert(hs_is_in(hd, "testing", 7));


    puts("hs_test: pop, copy_on_write");
    size_t val_size;
    void *val = hs_pop(hd, &val_size);
    assert(cmp(val, val_size, "test", 4) || cmp(val, val_size, "testing", 7));
    free(val);

    puts("hs_test: free");
    hs_free(ha);
    hs_free(hb);
    hs_free(hc);
    hs_free(hd);

    puts("hs_test: done");
}

