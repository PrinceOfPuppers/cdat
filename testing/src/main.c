#include <cdat/linked-list.h>

#include <stdint.h>
#include <assert.h>

#include <string.h>
#include <stdio.h>
#include <cdat/helpers.h>

typedef struct Hash_Set {
    // to be iterable
    Linked_List keys_ll;

    Linked_List *hs_arr;
    size_t hash_max;

} Hash_Set;

Hash_Set *hs_create(size_t hash_max){
    Hash_Set *hs = malloc_wrapper(sizeof(Hash_Set));
    hs->hash_max = hash_max;
    ll_init_empty(&hs->keys_ll);

    // init array of ll
    hs->hs_arr = malloc_wrapper(sizeof(Linked_List)*hash_max);

    // init all ll
    for(int i = 0; i < hash_max; i++){
        ll_init_empty(&(hs->hs_arr)[i]);
    }

    return hs;
}


int hs_is_in(Hash_Set *hs, void *val, size_t val_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    return ll_is_in(&hs->hs_arr[h], val, val_size);
}

// returns 1 if added, 0 if already exists
int hs_add(Hash_Set *hs, void *val, size_t val_size){
    uint32_t h = hash_digit_fold(val, val_size, hs->hash_max);
    if(!ll_is_in(&hs->hs_arr[h], val, val_size)){
        ll_append(&hs->hs_arr[h], val, val_size);
        ll_append(&hs->keys_ll, val, val_size);
        return 1;
    }
    return 0;
}

// TODO: distinction between copy and non-copy hash sets is required, otherwise freeing resources becomes a nighmare
// returns 1 if added, 0 if already exists
int hs_add_copy(Hash_Set *hs, void *val, size_t val_size){
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

void hs_from_ll(Hash_Set *hs, Linked_List *ll){
    //Hash_Set *hs = hs_create(hash_max);
    Linked_List_Node *n = ll->head;

    while(n != NULL){
        hs_add(hs, n->val, n->val_size);
        n = n->next;
    }
}

void hs_copy_from_ll(Hash_Set *hs, Linked_List *ll){
    //Hash_Set *hs = hs_create(hash_max);
    Linked_List_Node *n = ll->head;

    while(n != NULL){
        hs_add_copy(hs, n->val, n->val_size);
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

Hash_Set *hs_union(Hash_Set *hs1, Hash_Set *hs2){
    Hash_Set *h = hs_create(hs1->hash_max > hs2->hash_max ? hs1->hash_max : hs2->hash_max);
    hs_from_ll(h, &hs1->keys_ll);
    hs_from_ll(h, &hs2->keys_ll);
    return h;
}

Hash_Set *hs_union_copy(Hash_Set *hs1, Hash_Set *hs2){
    Hash_Set *h = hs_create(hs1->hash_max > hs2->hash_max ? hs1->hash_max : hs2->hash_max);
    hs_copy_from_ll(h, &hs1->keys_ll);
    hs_copy_from_ll(h, &hs2->keys_ll);
    return h;
}

Hash_Set *hs_intersection(Hash_Set *hs1, Hash_Set *hs2){
    Hash_Set *h = hs_create(hs1->hash_max > hs2->hash_max ? hs1->hash_max : hs2->hash_max);

    Linked_List_Node *n = hs1->keys_ll.head;
    while(n != NULL){
        if(ll_is_in(&hs2->keys_ll, n->val, n->val_size)){
            hs_add(h, n->val, n->val_size);
        }
        n = n->next;
    }

    return h;
}
Hash_Set *hs_intersection_copy(Hash_Set *hs1, Hash_Set *hs2){
    Hash_Set *h = hs_create(hs1->hash_max > hs2->hash_max ? hs1->hash_max : hs2->hash_max);

    Linked_List_Node *n = hs1->keys_ll.head;
    while(n != NULL){
        if(ll_is_in(&hs2->keys_ll, n->val, n->val_size)){
            hs_add_copy(h, n->val, n->val_size);
        }
        n = n->next;
    }

    return h;
}



int main(){
    ll_test();

    char *x = "testing";
    char *y = "hello there mister how are you I am really good thankyou for asking";
    uint32_t h = hash_digit_fold(x, strlen(x), 50000);
    printf("%i\n", h);
    h = hash_digit_fold(y, strlen(y), 100000000);
    printf("%i\n", h);
}
