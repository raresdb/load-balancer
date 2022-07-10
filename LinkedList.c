#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

linked_list_t*
ll_create(unsigned int data_size)
{
    linked_list_t* ll;
    ll = malloc(sizeof(linked_list_t));
    DIE(!ll, "Memory error");
    ll -> data_size = data_size;
    ll -> size = 0;
    ll -> head = NULL;
    return ll;
}

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    unsigned int i;
    ll_node_t* prev_node;
    ll_node_t* curr_node;
    prev_node = list -> head;
    curr_node = malloc(sizeof(ll_node_t));
    DIE(!curr_node, "Memory error");

	/* Index starts from 0 */
    if (n == 0 || list -> size == 0)
    {
        curr_node -> next = list -> head;
        list -> head = curr_node;
    }
    else
    {
		/* If the given position is too high, the element will be added at the
		end of the list */
        if (n >= list -> size)
        {
            n = list -> size;
        }

        for (i = 1; i < n; i++)
        {
            prev_node = prev_node -> next;
        }

        curr_node -> next = prev_node -> next;
        prev_node -> next = curr_node;
    }

	/* The data is copied from the block of memory pointed by new_data */
    curr_node -> data = malloc(list -> data_size);
    DIE(!curr_node -> data, "Memory error");
    memcpy(curr_node -> data, new_data, list -> data_size);
    list -> size++;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    unsigned int i;
    ll_node_t* prev_node;
    ll_node_t* curr_node;
    prev_node = list -> head;

	/* Index starts from 0 */
    if (n == 0 || list -> size == 1)
    {
        curr_node = list -> head;
        list -> head = list -> head -> next;
    }
    else
    {
		/* If n is too high, the elimination will occur for the last element of
		the list */
        if (n >= list -> size)
        {
            n = list -> size;
        }

        for (i = 1; i < n; i++)
        {
            prev_node = prev_node -> next;
        }

		curr_node = prev_node -> next;
        prev_node -> next = curr_node -> next;
    }

    list -> size--;
	/* Returning a pointer to the memory block where the node is being stored
	 * The function does not free this memory */
    return curr_node;
}

void
ll_free(linked_list_t** pp_list)
{
    int i;
    ll_node_t* address_list[(*pp_list) -> size];
    address_list[0] = (*pp_list) -> head;
    for (i = 1; i < (*pp_list) -> size; i++)
    {
        address_list[i] = address_list[i - 1] -> next;
    }

    for (i = (*pp_list) -> size - 1; i >= 0; i--)
    {
        free(address_list[i] -> data);
        free(address_list[i]);
    }

    free(*pp_list);
	/* Marking the fact that the list has been erased from the memory */
    *pp_list = NULL;
}

void
ll_print_string(linked_list_t* list)
{
    unsigned int i;
    ll_node_t* curr_node;
    curr_node = list -> head;

    for (i = 0; i < list -> size; i++)
    {
        printf("%s ", (char*)(curr_node -> data));
        curr_node = curr_node -> next;
    }

    printf("\n");
}
