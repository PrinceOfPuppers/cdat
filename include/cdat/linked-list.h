#ifndef CDAT_LINKED_LIST_H_
#define CDAT_LINKED_LIST_H_

#include <stdlib.h>
#include <cdat/helpers.h>



typedef struct Linked_List_Node {
    void *val;
    size_t val_size;
    struct Linked_List_Node *next;
    struct Linked_List_Node *prev;
} Linked_List_Node;

Linked_List_Node *ll_node_create(void *val, size_t val_size, Linked_List_Node *next, Linked_List_Node *prev);



typedef struct Linked_List {
    Linked_List_Node *head;
    Linked_List_Node *tail;
    size_t len;

    Linked_List_Node *_next;

    int copy_on_write;
    cdat_cmp_func cmp;
} Linked_List;

void ll_init_empty(Linked_List *ll, cdat_cmp_func cmp, int copy_on_write);
// values point to arr
Linked_List *ll_from_arr(void *arr, size_t arr_len, size_t val_size, cdat_cmp_func cmp, int copy_on_write);
//Linked_List *ll_copy_from_arr(void *arr, size_t arr_len, size_t val_size);
//void *ll_copy_to_arr(Linked_List *ll);
void *ll_to_arr(Linked_List *ll);

// free ll and its nodes, does not touch values
void ll_free_keep_vals(Linked_List *ll);
// free ll and its nodes, frees vals
void ll_free_free_vals(Linked_List *ll);
// if copy_on_write, free vals and ll, else just free ll
void ll_free(Linked_List *ll);


Linked_List_Node *ll_next(Linked_List *ll);
void ll_iter_reset(Linked_List *ll);

void *ll_get(Linked_List *ll, int index, size_t *out_size);

void ll_append(Linked_List *ll, void *val, size_t val_size);
void ll_push(Linked_List *ll, void *val, size_t val_size);
void ll_insert(Linked_List *ll, void *val, size_t val_size, int index);

void *ll_pop_back(Linked_List *ll, size_t *out_size);
void *ll_pop_front(Linked_List *ll, size_t *out_size);
void *ll_remove(Linked_List *ll, int index, size_t *out_size);

int ll_is_in(Linked_List *ll, void *val, size_t val_size);
void *ll_try_pop_val(Linked_List *ll, void *val, size_t val_size, size_t *out_size);
int ll_try_free_val(Linked_List *ll, void *val, size_t val_size);

// uses cmp func of la
int ll_cmp_ll(Linked_List *la, Linked_List *lb);
int ll_cmp_arr(Linked_List *ll, void *arr, size_t arr_size, size_t val_size);

void ll_map( Linked_List *ll, cdat_map_func map);

void ll_test();

#endif
