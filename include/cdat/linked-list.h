#ifndef CDAT_LINKED_LIST_H_
#define CDAT_LINKED_LIST_H_

#include <stdlib.h>



typedef struct Linked_List_Node {
    struct Linked_List_Node *next;
    struct Linked_List_Node *prev;
    void *val;
} Linked_List_Node;

Linked_List_Node *ll_node_create(void *val, Linked_List_Node *next, Linked_List_Node *prev);



typedef struct Linked_List {
    Linked_List_Node *head;
    Linked_List_Node *tail;
    size_t len;

    Linked_List_Node *_next;
} Linked_List;

// values point to arr
Linked_List *ll_from_arr(void *arr, size_t arr_len, size_t element_size);
Linked_List *ll_copy_from_arr(void *arr, size_t arr_len, size_t element_size);
void *ll_copy_to_arr(Linked_List *ll, size_t element_size);

// destroys ll and its nodes, does not touch values
void ll_destroy_keep_vals(Linked_List *ll);
void ll_destroy_free_vals(Linked_List *ll);


Linked_List_Node *ll_next(Linked_List *ll);
void ll_iter_reset(Linked_List *ll);

void *ll_get(Linked_List *ll, int index);

void ll_append(Linked_List *ll, void *val);
void ll_append_copy(Linked_List *ll, void *val, size_t element_size);
void ll_push(Linked_List *ll, void *val);
void ll_push_copy(Linked_List *ll, void *val, size_t element_size);
void ll_insert(Linked_List *ll, void *val, int index);
void ll_insert_copy(Linked_List *ll, void *val, size_t element_size, int index);

void *ll_pop_back(Linked_List *ll);
void *ll_pop_front(Linked_List *ll);
void *ll_remove(Linked_List *ll, int index);

int ll_cmp_ll(Linked_List *la, Linked_List *lb, size_t element_size);
int ll_cmp_arr(Linked_List *ll, void *arr, size_t arr_size, size_t element_size);

void ll_test();

#endif
