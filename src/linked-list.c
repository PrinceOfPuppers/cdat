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



void ll_init_empty(Linked_List *ll, cdat_cmp_func cmp, int copy_on_write){
    ll->head = NULL;
    ll->tail = NULL;
    ll->len = 0;
    ll->copy_on_write = copy_on_write;
    ll->cmp = cmp;
}

// values point to arr
Linked_List *ll_from_arr(void *arr, size_t arr_len, size_t val_size, cdat_cmp_func cmp, int copy_on_write){
    Linked_List *ll = malloc_wrapper(sizeof(Linked_List));
    ll_init_empty(ll, cmp, copy_on_write);

    void *p;
    for(int i = 0; i < arr_len; i++){
        p = arr+i*val_size;
        ll_append(ll, p, val_size);
    }

    return ll;
}

void *ll_to_arr(Linked_List *ll){
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
void ll_free_keep_vals(Linked_List *ll){
    while(ll->len > 0){
        ll_pop_back(ll, NULL);
    }
    free(ll);
}

void ll_free_free_vals(Linked_List *ll){
    while(ll->len > 0){
        free(ll_pop_back(ll, NULL));
    }
    free(ll);
}

void ll_free(Linked_List *ll){
    if(ll->copy_on_write){
        return ll_free_free_vals(ll);
    }
    return ll_free_keep_vals(ll);
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

    assert(index >= 0);
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


static void _ll_append_ref(Linked_List *ll, void *val, size_t val_size){
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
static void _ll_append_copy(Linked_List *ll, void *val, size_t val_size){
    void *v = malloc_wrapper(val_size);
    memcpy(v, val, val_size);
    _ll_append_ref(ll, v, val_size);
}

void ll_append(Linked_List *ll, void *val, size_t val_size){
    if(ll->copy_on_write){
        return _ll_append_copy(ll, val, val_size);
    }
    return _ll_append_ref(ll, val, val_size);
}

static void _ll_push_ref(Linked_List *ll, void *val, size_t val_size){
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
static void _ll_push_copy(Linked_List *ll, void *val, size_t val_size){
    void *v = malloc_wrapper(val_size);
    memcpy(v, val, val_size);
    _ll_push_ref(ll, v, val_size);
}
void ll_push(Linked_List *ll, void *val, size_t val_size){
    if(ll->copy_on_write){
        return _ll_push_copy(ll, val, val_size);
    }
    return _ll_push_ref(ll, val, val_size);
}

static void _ll_insert_ref(Linked_List *ll, void *val, size_t val_size, int index){
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
static void _ll_insert_copy(Linked_List *ll, void *val, size_t val_size, int index){
    void *v = malloc_wrapper(val_size);
    memcpy(v, val, val_size);
    _ll_insert_ref(ll, v, val_size, index);
}
void ll_insert(Linked_List *ll, void *val, size_t val_size, int index){
    if(ll->copy_on_write){
        return _ll_insert_copy(ll, val, val_size, index);
    }
    return _ll_insert_ref(ll, val, val_size, index);
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
        if(ll->cmp(n->val, n->val_size, val, val_size)){
            return 1;
        }
        n = n->next;
    }
    return 0;
}

void *ll_try_pop_val(Linked_List *ll, void *val, size_t val_size, size_t *out_size){
    Linked_List_Node *n = ll->head;
    int i = 0;
    while(n != NULL){
        if(ll->cmp(n->val, n->val_size, val, val_size)){
            void *res = ll_remove(ll, i, out_size);
            return res;
        }
        i++;
        n = n->next;
    }
    return NULL;
}
int ll_try_free_val(Linked_List *ll, void *val, size_t val_size){
    void *x = ll_try_pop_val(ll, val, val_size, NULL);
    if(x != NULL){
        free(x);
        return 1;
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

        if(!la->cmp(na->val, na->val_size, nb->val, nb->val_size)){
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
        if((n == NULL) || !ll->cmp(n->val, n->val_size, arr+i*val_size, val_size)){
            return 0;
        }

        n = n->next;
    }
    assert(n == NULL);
    return 1;
}

void ll_map( Linked_List *ll, cdat_map_func map ){
    Linked_List_Node *n = ll->head;
    int i = 0;
    while(n != NULL){
        (*map)(i, n->val, n->val_size);
        n = n->next;
        i++;
    }
}


static int _cmp_int(void *va, size_t va_size, void *vb, size_t vb_size){
    if(va_size!=vb_size){
        return 0;
    }
    return *(int *)va == *(int *)vb;
}

static void _ll_test_map(int index, void *val, size_t val_size){
    assert(val_size == sizeof(int));
    *(int *)val = index;
}


void ll_test(){
    puts("ll_test: creation from array, no copy on write");
    int x[] = {0, 1, 2, 3, 4};
    cdat_cmp_func cmp = _cmp_int;
    Linked_List *la = ll_from_arr(x, 5, sizeof(int), cmp, 0);
    assert(ll_cmp_arr(la, (int[]){0,1,2,3,4}, 5, sizeof(int)));

    x[2] = 6;
    assert(ll_cmp_arr(la, (int[]){0,1,6,3,4}, 5, sizeof(int)));
    x[2] = 2;

    puts("ll_test: insertion, no copy on write");
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


    puts("ll_test: append and push, no copy on write");
    y = 6;
    ll_push(la, &y, sizeof(int));
    ll_append(la, &y, sizeof(int));
    assert(ll_cmp_arr(la, (int[]){6,0,1,2,3,4,6}, 7, sizeof(int)));
    y = 3;
    assert(ll_cmp_arr(la, (int[]){3,0,1,2,3,4,3}, 7, sizeof(int)));

    puts("ll_test: pop front and back, no copy on write");
    assert(*(int *)ll_pop_front(la, NULL) == 3);
    assert(*(int *)ll_pop_back(la, NULL) == 3);
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

    puts("ll_test: creation from array, copy on write");
    int z[] = {6, 7, 8, 9, 10};
    Linked_List *lb = ll_from_arr(z, 5, sizeof(int), cmp, 1);
    assert(ll_cmp_arr(lb, (int[]){6, 7, 8, 9, 10}, 5, sizeof(int)));
    z[3] = 0;
    assert(ll_cmp_arr(lb, (int[]){6, 7, 8, 9, 10}, 5, sizeof(int)));

    puts("ll_test: append and push, copy on write");
    y = 6;
    ll_push(lb, &y, sizeof(int));
    ll_append(lb, &y, sizeof(int));
    assert(ll_cmp_arr(lb, (int[]){6,6,7,8,9,10,6}, 7, sizeof(int)));
    y = 3;
    assert(ll_cmp_arr(lb, (int[]){6,6,7,8,9,10,6}, 7, sizeof(int)));

    puts("ll_test: is in on write");
    assert(ll_cmp_arr(lb, (int[]){6,6,7,8,9,10,6}, 7, sizeof(int)));
    int a = 7;
    assert(ll_is_in(lb, &a, sizeof(int)));

    puts("ll_test: pop front and back, copy on write");
    assert(*(int *)ll_pop_front(lb, NULL) == 6);
    assert(*(int *)ll_pop_back(lb, NULL) == 6);
    assert(ll_cmp_arr(lb, (int[]){6, 7, 8, 9, 10}, 5, sizeof(int)));


    puts("ll_test: map, copy on write");
    cdat_map_func map = *_ll_test_map;
    ll_map(lb, map);
    assert(ll_cmp_arr(lb, (int[]){0,1,2,3,4}, 5, sizeof(int)));

    puts("ll_test: ll_cmp_ll");
    assert(ll_cmp_ll(la, lb));

    puts("ll_test: freeing");
    ll_free(la);
    ll_free(lb);

    puts("ll_test: done");
}
