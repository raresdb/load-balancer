#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

/* Function used for memory allocation and initialisation of a linked list */
linked_list_t*
ll_create(unsigned int data_size);

/* Function used for adding a new element to the nth position of the list */
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* data);

/* Function used for removing the nth element from the list */
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n);

/* Function used for deallocating all the memory used by the list */
void
ll_free(linked_list_t** pp_list);

/* Function used for printing a string list */
void
ll_print_string(linked_list_t* list);

#endif /* LINKEDLIST_H_ */
