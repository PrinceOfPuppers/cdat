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


int hs_try_remove(Hash_Set *hs, void *val, size_t val_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    if(ll_try_remove_val(&hs->hs_arr[h], val, val_size)){
        assert(ll_try_remove_val(&hs->keys_ll, val, val_size));
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

void hs_test(){
    puts("hs_test: creation, no copy on write");
    /*
    Hash_Set *ha = hs_create(10000, 0);

    
    hs_add(ha, )

    puts("hs_test: creation from array, no copy on write");

    puts("hs_test: done");
    */
}

