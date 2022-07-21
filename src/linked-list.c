#include <cdat/linked-list.h>
#include <cdat/helpers.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>


Linked_List_Node *ll_node_create(void *val, size_t val_size, Linked_List_Node *next, Linked_List_Node *prev){
    Linked_List_Node *n = malloc_wrapper(sizeof(Linked_List_Node));
    n->val = val;
    n->val_size = val_size;

    n->next = next;
    n->prev = prev;

    return n;
}



void ll_init_empty(Linked_List *ll){
    ll->head = NULL;
    ll->tail = NULL;
    ll->len = 0;
    
}

// values point to arr
Linked_List *ll_from_arr(void *arr, size_t arr_len, size_t val_size){
    Linked_List *ll = malloc_wrapper(sizeof(Linked_List));
    ll->len = 0;

    void *p;
    for(int i = 0; i < arr_len; i++){
        p = arr+i*val_size;
        ll_append(ll, p, val_size);
    }

    return ll;
}

Linked_List *ll_copy_from_arr(void *arr, size_t arr_len, size_t val_size){
    Linked_List *ll = malloc_wrapper(sizeof(Linked_List));
    ll->len = 0;

    void *p;
    void *v;
    for(int i = 0; i < arr_len; i++){
        v = malloc_wrapper(val_size);
        p = arr+i*val_size;
        memcpy(v, p, val_size);
        ll_append(ll, v, val_size);
    }

    return ll;
}

void *ll_copy_to_arr(Linked_List *ll){
    if(ll->len == 0){
        return NULL;
    }
    size_t val_size = ll->head->val_size;
    void *arr = malloc_wrapper(ll->len*val_size);

    Linked_List_Node *n = ll->head;
    void *p;
    for(int i = 0; i < ll->len; i++){
        assert(n != NULL);
        assert(n->val_size == val_size);
        p = arr+i*val_size;
        memcpy(p, n->val, val_size);
        n = n->next;
    }

    return arr;
}


// destroys ll and its nodes, does not touch values
void ll_destroy_keep_vals(Linked_List *ll){
    while(ll->len > 0){
        ll_pop_back(ll, NULL);
    }
    free(ll);
}

void ll_destroy_free_vals(Linked_List *ll){
    while(ll->len > 0){
        free(ll_pop_back(ll, NULL));
    }
    free(ll);
}


Linked_List_Node *ll_next(Linked_List *ll){
    Linked_List_Node *n = ll->_next;
    if(ll->_next != NULL){
        ll->_next = ll->_next->next;
    }
    return n;
}
void ll_iter_reset(Linked_List *ll){
    ll->_next = ll->head;
}


static int _process_ll_index(Linked_List *ll, int index){
    assert(ll->head != NULL);
    index = index < 0 ? ll->len + index : index;

    assert(index > 0);
    assert(index < ll->len);

    return index;
}
static Linked_List_Node *_ll_get_node(Linked_List *ll, int index){
    index = _process_ll_index(ll, index);

    Linked_List_Node *n;
    if(index<ll->len/2){
        n = ll->head;
        for(int i = 0; i < index; i++){
            n = n->next;
        }
    }
    else{
        n = ll->tail;
        for(int i = ll->len-1; i > index; i--){
            n = n->prev;
        }
    }
    return n;
}


void ll_append(Linked_List *ll, void *val, size_t val_size){
    Linked_List_Node *n;
    if(ll->tail == NULL){
        n = ll_node_create(val, val_size, NULL, NULL);
        ll->head = n;
        ll->tail = n;
        ll->len = 1;
    }
    else{
        n = ll_node_create(val, val_size, NULL, ll->tail);
        ll->tail->next = n;
        ll->tail = n;
        ll->len+=1;
    }
}
void ll_append_copy(Linked_List *ll, void *val, size_t val_size){
    void *v = malloc_wrapper(val_size);
    memcpy(v, val, val_size);
    ll_append(ll, v, val_size);
}

void ll_push(Linked_List *ll, void *val, size_t val_size){
    Linked_List_Node *n;
    if(ll->head == NULL){
        n = ll_node_create(val, val_size, NULL, NULL);
        ll->head = n;
        ll->tail = n;
        ll->len = 1;
    }
    else{
        n = ll_node_create(val, val_size, ll->head, NULL);
        ll->head->prev = n;
        ll->head = n;
        ll->len++;
    }
}
void ll_push_copy(Linked_List *ll, void *val, size_t val_size){
    void *v = malloc_wrapper(val_size);
    memcpy(v, val, val_size);
    ll_push(ll, v, val_size);
}

void ll_insert(Linked_List *ll, void *val, size_t val_size, int index){
    index = _process_ll_index(ll, index);

    if(ll->len == 0){
        Linked_List_Node *n = ll_node_create(val, val_size, NULL, NULL);
        ll->head = n;
        ll->tail = n;
        ll->len = 1;
        return;
    }

    // ll has at least 1 node
    if(index == 0){
        Linked_List_Node *n = ll_node_create(val, val_size, ll->head, NULL);
        ll->head->prev = n;
        ll->head = n;
        ll->len++;
        return;
    }

    Linked_List_Node *prev_n = _ll_get_node(ll, index-1);
    Linked_List_Node *n = ll_node_create(val, val_size, NULL, prev_n);
    Linked_List_Node *next_n = prev_n->next;
    // inserted at the end
    if(next_n == NULL){
        ll->tail = n;
    }
    else{
        n->next = next_n;
        next_n->prev = n;
    }
    prev_n->next = n;
    ll->len++;
}
void ll_insert_copy(Linked_List *ll, void *val, size_t val_size, int index){
    void *v = malloc_wrapper(val_size);
    memcpy(v, val, val_size);
    ll_insert(ll, v, val_size, index);
}

void *ll_pop_back(Linked_List *ll, size_t *out_size){
    assert(ll->head != NULL);
    assert(ll->len > 0);

    Linked_List_Node *n = ll->tail;
    if(n->prev == NULL){
        ll->head = NULL;
        ll->tail = NULL;
        ll->len = 0;
    }
    else{
        ll->tail = n->prev;
        n->prev->next = NULL;
        ll->len--;
    }

    if(out_size != NULL){
        *out_size = n->val_size;
    }
    void *v = n->val;
    free(n);
    return v;

}

void *ll_pop_front(Linked_List *ll, size_t *out_size){
    assert(ll->head != NULL);
    assert(ll->len > 0);

    Linked_List_Node *n = ll->head;
    if(n->next == NULL){
        ll->head = NULL;
        ll->tail = NULL;
        ll->len = 0;
    }
    else{
        ll->head = n->next;
        n->next->prev = NULL;
        ll->len--;
    }

    if(out_size != NULL){
        *out_size = n->val_size;
    }
    void *v = n->val;
    free(n);
    return v;
}



void *ll_remove(Linked_List *ll, int index, size_t *out_size){
    index = _process_ll_index(ll, index);
    Linked_List_Node *n = _ll_get_node(ll, index);
    if(ll->len == 1){
        ll->head = NULL;
        ll->tail = NULL;
        ll->len = 0;
    }
    else{
        // if n is last
        if(n->next == NULL){
            ll->tail = n->prev;
        }
        else{
            n->next->prev = n->prev;
        }

        // if n is first
        if(n->prev == NULL){
            ll->head = n->next;
        }
        else{
            n->prev->next = n->next;
        }

        ll->len--;
    }

    if(out_size != NULL){
        *out_size = n->val_size;
    }
    void *v = n->val;
    free(n);
    return v;
}


void *ll_get(Linked_List *ll, int index, size_t *out_size){
    Linked_List_Node *n = _ll_get_node(ll, index);

    if(out_size != NULL){
        *out_size = n->val_size;
    }
    return n->val;
}

int ll_is_in(Linked_List *ll, void *val, size_t val_size){
    Linked_List_Node *n = ll->head;
    while(n != NULL){
        if(val_size == n->val_size){
            if (memcmp(n->val, val, val_size) == 0){
                return 1;
            }
        }
        n = n->next;
    }
    return 0;
}

int ll_try_remove_val(Linked_List *ll, void *val, size_t val_size){
    Linked_List_Node *n = ll->head;
    int i = 0;
    while(n != NULL){
        if(val_size == n->val_size){
            if (memcmp(n->val, val, val_size) == 0){
                ll_remove(ll, i, NULL);
                return 1;
            }
        }
        i++;
        n = n->next;
    }
    return 0;
}


int ll_cmp_ll(Linked_List *la, Linked_List *lb){
    Linked_List_Node *na = la->head;
    Linked_List_Node *nb = lb->head;

    while(1){
        if(na == NULL){
            return nb == NULL;
        }
        if(nb == NULL){
            return na == NULL;
        }

        if(na->val_size != nb->val_size){
            return 0;
        }

        if (memcmp(na->val, nb->val, na->val_size) != 0){
            return 0;
        }
        na = na->next;
        nb = nb->next;
    }
}

int ll_cmp_arr(Linked_List *ll, void *arr, size_t arr_size, size_t val_size){
    if(arr_size != ll->len){
        return 0;
    }
    if(ll->len == 0){
        return 1;
    }

    Linked_List_Node *n = ll->head;

    for(int i = 0; i < arr_size; i++){
        if((n == NULL) || n->val_size != val_size){
            return 0;
        }

        if (memcmp(n->val, arr+i*val_size, val_size) != 0){
            return 0;
        }

        n = n->next;
    }
    assert(n == NULL);
    return 1;
}

void ll_test(){
    puts("ll_test: creation from array, no copy");
    int x[] = {0, 1, 2, 3, 4};
    Linked_List *la = ll_from_arr(x, 5, sizeof(int));
    assert(ll_cmp_arr(la, (int[]){0,1,2,3,4}, 5, sizeof(int)));

    x[2] = 6;
    assert(ll_cmp_arr(la, (int[]){0,1,6,3,4}, 5, sizeof(int)));
    x[2] = 2;

    puts("ll_test: insertion, no copy");
    int y = 10;
    ll_insert(la, &y, sizeof(int), 2);
    assert(ll_cmp_arr(la, (int[]){0,1,10,2,3,4}, 6, sizeof(int)));
    y = 3;
    assert(ll_cmp_arr(la, (int[]){0,1,3,2,3,4}, 6, sizeof(int)));

    puts("ll_test: removal");
    size_t tmp;
    assert(*(int *)ll_remove(la, 2, &tmp) == 3);
    assert(tmp == sizeof(int));
    assert(ll_cmp_arr(la, (int[]){0,1,2,3,4}, 5, sizeof(int)));


    puts("ll_test: append and push with copy");
    y = 6;
    ll_push_copy(la, &y, sizeof(int));
    ll_append_copy(la, &y, sizeof(int));
    assert(ll_cmp_arr(la, (int[]){6,0,1,2,3,4,6}, 7, sizeof(int)));
    y = 3;
    assert(ll_cmp_arr(la, (int[]){6,0,1,2,3,4,6}, 7, sizeof(int)));

    puts("ll_test: pop front and back");
    assert(*(int *)ll_pop_front(la, NULL) == 6);
    assert(*(int *)ll_pop_back(la, NULL) == 6);
    assert(ll_cmp_arr(la, (int[]){0,1,2,3,4}, 5, sizeof(int)));

    puts("ll_test: get");
    assert(*(int *)ll_get(la, 3, NULL) == 3);

    puts("ll_test: iter");
    Linked_List_Node *n;
    ll_iter_reset(la);
    int i = 0;
    while( (n = ll_next(la)) ){
        assert(*(int *)n->val == i);
        i++;
    }

    puts("ll_test: creation from array, copy");
    int z[] = {6, 7, 8, 9, 10};
    Linked_List *lb = ll_copy_from_arr(z, 5, sizeof(int));
    assert(ll_cmp_arr(lb, (int[]){6, 7, 8, 9, 10}, 5, sizeof(int)));
    z[3] = 0;
    assert(ll_cmp_arr(lb, (int[]){6, 7, 8, 9, 10}, 5, sizeof(int)));

    puts("ll_test: freeing");
    ll_destroy_keep_vals(la);
    ll_destroy_free_vals(lb);

    //TODO: add ll_is_in test

    puts("ll_test: done");
}
