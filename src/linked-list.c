#include <cdat/linked-list.h>
#include <cdat/helpers.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>


Linked_List_Node *ll_node_create(void *val, Linked_List_Node *next, Linked_List_Node *prev){
    Linked_List_Node *n = malloc_wrapper(sizeof(Linked_List_Node));
    n->next = next;
    n->prev = prev;
    n->val = val;
    return n;
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


void ll_append(Linked_List *ll, void *val){
    Linked_List_Node *n;
    if(ll->tail == NULL){
        n = ll_node_create(val, NULL, NULL);
        ll->head = n;
        ll->tail = n;
        ll->len = 1;
    }
    else{
        n = ll_node_create(val, NULL, ll->tail);
        ll->tail->next = n;
        ll->tail = n;
        ll->len+=1;
    }
}
void ll_append_copy(Linked_List *ll, void *val, size_t element_size){
    void *v = malloc_wrapper(element_size);
    memcpy(v, val, element_size);
    ll_append(ll, v);
}

void ll_push(Linked_List *ll, void *val){
    Linked_List_Node *n;
    if(ll->head == NULL){
        n = ll_node_create(val, NULL, NULL);
        ll->head = n;
        ll->tail = n;
        ll->len = 1;
    }
    else{
        n = ll_node_create(val, ll->head, NULL);
        ll->head->prev = n;
        ll->head = n;
        ll->len++;
    }
}
void ll_push_copy(Linked_List *ll, void *val, size_t element_size){
    void *v = malloc_wrapper(element_size);
    memcpy(v, val, element_size);
    ll_push(ll, v);
}

void ll_insert(Linked_List *ll, void *val, int index){
    index = _process_ll_index(ll, index);

    if(ll->len == 0){
        Linked_List_Node *n = ll_node_create(val, NULL, NULL);
        ll->head = n;
        ll->tail = n;
        ll->len = 1;
        return;
    }

    // ll has at least 1 node
    if(index == 0){
        Linked_List_Node *n = ll_node_create(val, ll->head, NULL);
        ll->head->prev = n;
        ll->head = n;
        ll->len++;
        return;
    }

    Linked_List_Node *prev_n = _ll_get_node(ll, index-1);
    Linked_List_Node *n = ll_node_create(val, NULL, prev_n);
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
void ll_insert_copy(Linked_List *ll, void *val, size_t element_size, int index){
    void *v = malloc_wrapper(element_size);
    memcpy(v, val, element_size);
    ll_insert(ll, v, index);
}

void *ll_pop_back(Linked_List *ll){
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
    void *val = n->val;
    free(n);
    return val;
}

void *ll_pop_front(Linked_List *ll){
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
    void *val = n->val;
    free(n);
    return val;
}



void *ll_remove(Linked_List *ll, int index){
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


    void *val = n->val;
    free(n);
    return val;
}


void *ll_get(Linked_List *ll, int index){
    Linked_List_Node *n = _ll_get_node(ll, index);
    return n->val;
}

// values point to arr
Linked_List *ll_from_arr(void *arr, size_t arr_len, size_t element_size){
    Linked_List *ll = malloc_wrapper(sizeof(Linked_List));
    ll->len = 0;

    void *p;
    for(int i = 0; i < arr_len; i++){
        p = arr+i*element_size;
        ll_append(ll, p);
    }

    return ll;
}

Linked_List *ll_copy_from_arr(void *arr, size_t arr_len, size_t element_size){
    Linked_List *ll = malloc_wrapper(sizeof(Linked_List));
    ll->len = 0;

    void *p;
    void *v;
    for(int i = 0; i < arr_len; i++){
        v = malloc_wrapper(element_size);
        p = arr+i*element_size;
        memcpy(v, p, element_size);
        ll_append(ll, v);
    }

    return ll;
}

void *ll_copy_to_arr(Linked_List *ll, size_t element_size){
    if(ll->len == 0){
        return NULL;
    }
    void *arr = malloc_wrapper(ll->len*element_size);

    Linked_List_Node *n = ll->head;
    void *p;
    for(int i = 0; i < ll->len; i++){
        assert(n != NULL);
        p = arr+i*element_size;
        memcpy(p, n->val, element_size);
        n = n->next;
    }

    return arr;
}

// destroys ll and its nodes, does not touch values
void ll_destroy_keep_vals(Linked_List *ll){
    while(ll->len > 0){
        ll_pop_back(ll);
    }
    free(ll);
}

void ll_destroy_free_vals(Linked_List *ll){
    while(ll->len > 0){
        free(ll_pop_back(ll));
    }
    free(ll);
}

int ll_cmp_ll(Linked_List *la, Linked_List *lb, size_t element_size){
    Linked_List_Node *na = la->head;
    Linked_List_Node *nb = lb->head;

    while(1){
        if(na == NULL){
            return nb == NULL;
        }
        if(nb == NULL){
            return na == NULL;
        }

        if (memcmp(na->val, nb->val, element_size) != 0){
            return 0;
        }
        na = na->next;
        nb = nb->next;
    }
}

int ll_cmp_arr(Linked_List *ll, void *arr, size_t arr_size, size_t element_size){
    if(arr_size != ll->len){
        return 0;
    }

    Linked_List_Node *n = ll->head;

    for(int i = 0; i < arr_size; i++){
        assert(n != NULL);

        if (memcmp(n->val, arr+i*element_size, element_size) != 0){
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
    ll_insert(la, &y, 2);
    assert(ll_cmp_arr(la, (int[]){0,1,10,2,3,4}, 6, sizeof(int)));
    y = 3;
    assert(ll_cmp_arr(la, (int[]){0,1,3,2,3,4}, 6, sizeof(int)));

    puts("ll_test: removal");
    assert(*(int *)ll_remove(la, 2) == 3);
    assert(ll_cmp_arr(la, (int[]){0,1,2,3,4}, 5, sizeof(int)));

    puts("ll_test: append and push with copy");
    y = 6;
    ll_push_copy(la, &y, sizeof(int));
    ll_append_copy(la, &y, sizeof(int));
    assert(ll_cmp_arr(la, (int[]){6,0,1,2,3,4,6}, 7, sizeof(int)));
    y = 3;
    assert(ll_cmp_arr(la, (int[]){6,0,1,2,3,4,6}, 7, sizeof(int)));

    puts("ll_test: pop front and back");
    assert(*(int *)ll_pop_front(la) == 6);
    assert(*(int *)ll_pop_back(la) == 6);
    assert(ll_cmp_arr(la, (int[]){0,1,2,3,4}, 5, sizeof(int)));

    puts("ll_test: get");
    assert(*(int *)ll_get(la, 3) == 3);

    puts("ll_test: iter");
    Linked_List_Node *n;
    ll_iter_reset(la);
    int i = 0;
    while( (n = ll_next(la)) ){
        assert(*(int *)n->val == i);
        i++;
    }

    // TODO: test ll_copy_from_arr
    //Linked_List *lb = ll_copy_from_arr(x, 5, sizeof(int));
    //ll_cmp_arr(la, {})
}
