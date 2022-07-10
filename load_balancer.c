#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "LinkedList.h"
#include "Hashtable.h"
#include "utils.h"

struct load_balancer {
	hashtable_t* server_list;
	unsigned int hash_ring[SERVERS_MAX_NUMBER * 3];
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}


load_balancer* init_load_balancer() {
	load_balancer* main =  malloc(sizeof(load_balancer));
	DIE(!main, "Error when allocating memory for the load balancer!");

	main -> server_list = ht_create(MAX_BUCKET_SIZE, hash_function_int,
								   compare_function_ints);
    return main;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
	/* aux - used as a block of memory where a value - that
	needs to be given through a pointer - can be stored */
	unsigned i, aux;
	server_memory* server;

	/* Finding the position of the server*/
	for (i = 0; i < main -> server_list -> size * 3; i++)
	{
		if (hash_function_servers(&main -> hash_ring[i]) > hash_function_key(key))
		{
			break;
		}
	}

	/* If the position found is greater than the position of the last server,
	then the assignated server will be the first one */
	if (i == main -> server_list -> size * 3)
	{
		i = 0;
	}

	aux = main -> hash_ring[i] - 100000;

	/* Finding the server id by using the label */
	if (ht_has_key(main -> server_list, &main -> hash_ring[i]))
	{
		*server_id = main -> hash_ring[i];
	}
	else if (ht_has_key(main -> server_list, &aux))
	{
		*server_id = main -> hash_ring[i] - 100000;
	}
	else
	{
		*server_id = main -> hash_ring[i] - 200000;
	}

	server = *(server_memory**)ht_get(main -> server_list, server_id);
	server_store(server, key, value);
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	unsigned i, aux;
	server_memory* server;

	for (i = 0; i < main -> server_list -> size * 3; i++)
	{
		if (hash_function_servers(&main -> hash_ring[i]) > hash_function_key(key))
		{
			break;
		}
	}

	if (i == main -> server_list -> size * 3)
	{
		i = 0;
	}

	aux = main -> hash_ring[i] - 100000;

	if (ht_has_key(main -> server_list, &main -> hash_ring[i]))
	{
		*server_id = main -> hash_ring[i];
	}
	else if (ht_has_key(main -> server_list, &aux))
	{
		*server_id = main -> hash_ring[i] - 100000;
	}
	else
	{
		*server_id = main -> hash_ring[i] - 200000;
	}

	server = *(server_memory**)ht_get(main -> server_list, server_id);

	/* Return the product information */
	return server_retrieve(server, key);
}

void loader_add_server(load_balancer* main, int server_id) {
	unsigned label, aux, neighbour_id;
	int i, j, k;
	server_memory* neighbours[3];
	server_memory* server = init_server_memory();

	ht_put(main -> server_list, &server_id, sizeof(int), &server,
		   sizeof(server_memory*));

	/* Adding all 3 labels into the hashring */
	for (i = 0; i < 3; i++)
	{
		label = server_id + i * 100000;

		/* Finding the position for the server*/
		for (j = 0; j < main -> server_list -> size * 3 + i - 3; j++)
		{
			if (hash_function_servers(&label) <
			   hash_function_servers(&main -> hash_ring[j]))
			{
				break;
			}
			else if (hash_function_servers(&label) ==
					hash_function_servers(&main -> hash_ring[j])
					&& label < main -> hash_ring[j])
			{
				break;
			}
		}

		for (k = main -> server_list -> size * 3 - 4 + i; k >= j;
			k--)
		{
			main -> hash_ring[k + 1] = main -> hash_ring[k];
		}

		main -> hash_ring[j] = label;

		/* Marking the neighbour as NULL in case the server is the only one in
		the list */
		if (!(main -> server_list -> size -1))
		{
			neighbours[i] = NULL;
			continue;
		}

		/* Marking the position of the previous server as -1 in case this one is
		the last one */
		if(j == main -> server_list -> size * 3 + i - 3)
		{
			j = -1;
		}

		aux = main -> hash_ring[j + 1] - 100000;

		if (ht_has_key(main -> server_list, &main -> hash_ring[j + 1]))
		{
			neighbour_id = main -> hash_ring[j + 1];
		}
		else if (ht_has_key(main -> server_list, &aux))
		{
			neighbour_id = aux;
		}
		else
		{
			neighbour_id = main -> hash_ring[j + 1] - 200000;
		}

		neighbours[i] = *(server_memory**)ht_get(main -> server_list, &neighbour_id);
	}

	for (int i = 0; i < 3; i++)
	{
		int flag = 1;

		/* It's pointless to check twice the same neighbour */
		for (int j = 0; j < i; j++)
		{
			if(neighbours[j] == neighbours[i])
			{
				flag = 0;
			}
		}

		/* Redistributing the products that have to be moved into the new
		server */
		if (flag && neighbours[i] != NULL)
		{
			for (unsigned k = 0; k < neighbours[i] -> data -> hmax; k++)
			{
				ll_node_t* node = neighbours[i] -> data -> buckets[k] -> head;

				for (unsigned j = 0; j < neighbours[i] -> data -> buckets[k] -> size; j++)
				{
					void* key = ((info*)(node -> data)) -> key;
					void* value = ((info*)(node -> data)) -> value;

					loader_store(main, key, value, &server_id);
					node = node -> next;
				}
			}
		}
	}
}


void loader_remove_server(load_balancer* main, int server_id) {
	int left_labels = 3; /* Labels left to be eliminated */

	/* Elimination of labels from the hash ring */
	for (unsigned i = 0; i < main -> server_list -> size * 3 - (3 - left_labels);
		i++)
	{
		if ((main -> hash_ring[i] - server_id) % 100000 == 0 &&
		   main -> hash_ring[i] - server_id < 300000)
		{
			left_labels--;

			for (unsigned j = i; j < main -> server_list -> size * 3 -
			    (left_labels - 3); j++)
			{
				main -> hash_ring[j] = main -> hash_ring[j + 1];
			}

			i--;
		}

		if (!left_labels)
		{
			break;
		}
	}

	/* Deallocating the server memory */
	server_memory* server = *(server_memory**)ht_get(main -> server_list,
													 &server_id);
	ht_remove_entry(main -> server_list, &server_id);

	for (unsigned i = 0; i < server -> data -> hmax; i++)
	{
		ll_node_t* node = server -> data -> buckets[i] -> head;

		for (unsigned j = 0; j < server -> data -> buckets[i] -> size; j++)
		{
			void* key = ((info*)(node -> data)) -> key;
			void* value = ((info*)(node -> data)) -> value;

			loader_store(main, key, value, &server_id);
			node = node -> next;
		}
	}

	free_server_memory(server);
}

void free_load_balancer(load_balancer* main) {
	unsigned int* server_id;

    for (unsigned i = 0; i < main -> server_list -> size * 3; i++)
	{
		server_id = &main -> hash_ring[i];

		if (ht_has_key(main -> server_list, server_id))
		{
			free_server_memory(*(server_memory**)ht_get(main -> server_list,
														server_id));
		}
	}

	ht_free(main -> server_list);
	free(main);
}
