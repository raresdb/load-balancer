#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "Hashtable.h"

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t* hash;
	unsigned i;

	hash = malloc(sizeof(hashtable_t));
	DIE(!hash, "Malloc error!");

	hash -> size = 0;
	hash -> hmax = hmax;
	hash -> hash_function = hash_function;
	hash -> compare_function = compare_function;
	hash -> buckets = malloc(hmax * sizeof(linked_list_t*));
	DIE(!hash -> buckets, "Malloc error!");

	for (i = 0; i < hmax; i++)
	{
		hash -> buckets[i] = malloc(sizeof(linked_list_t));
		DIE(!hash -> buckets[i], "Malloc error!");
		hash -> buckets[i] -> head = NULL;
		hash -> buckets[i] -> data_size = sizeof(info);
		hash -> buckets[i] -> size = 0;
	}

	return hash;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	info entry;
	unsigned i;
	unsigned index = (*ht -> hash_function)(key) % ht -> hmax;
	linked_list_t* bucket = ht -> buckets[index];
	ll_node_t* bucket_node = bucket -> head;

	if(!ht)
	{
		printf("Nonexistent hashtable!");
		return;
	}

	/* The data is given through a pointer to a block of memory containing
	information that has to be copied into the lists */
	entry.key = malloc(key_size);
	entry.value = malloc(value_size);
	DIE(!entry.key, "Malloc error!");
	DIE(!entry.value, "Malloc error!");
	memcpy(entry.key, key, key_size);
	memcpy(entry.value, value, value_size);

	for (i = 0; i < bucket -> size; i++)
	{
		if(!(*ht -> compare_function)(entry.key, ((info*)bucket_node -> data) ->
		   key))
		{
			memcpy(((info*)bucket_node -> data) -> value, entry.value,
				   value_size);
			free(entry.key);
			free(entry.value);
			return;
		}

		bucket_node = bucket_node -> next;
	}

	ll_add_nth_node(bucket, bucket -> size, &entry);
	ht -> size++;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned i;
	unsigned index = (*ht -> hash_function)(key) % ht -> hmax;
	linked_list_t* bucket = ht -> buckets[index];
	ll_node_t* bucket_node = bucket -> head;

	if (!ht)
	{
		printf("Nonexistent hashtable!");
		return NULL;
	}

	for (i = 0; i < bucket -> size; i++)
	{
		if(!(*ht -> compare_function)(key, ((info*)bucket_node -> data) -> key))
		{
			return ((info*)bucket_node -> data) -> value;
		}

		bucket_node = bucket_node -> next;
	}

	return NULL;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	unsigned i;
	unsigned index = (*ht -> hash_function)(key) % ht -> hmax;
	linked_list_t* bucket = ht -> buckets[index];
	ll_node_t* bucket_node = bucket -> head;

	if (!ht)
	{
		printf("Nonexistent hashtable!");
		/* Error code */
		return -1;
	}

	for (i = 0; i < bucket -> size; i++)
	{
		if(!(*ht -> compare_function)(key, ((info*)bucket_node -> data) -> key))
		{
			/* Value that confirms that the key has been added */
			return 1;
		}

		bucket_node = bucket_node -> next;
	}

	/* Value that states that the key has not been added */
	return 0;
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned i;
	unsigned index = (*ht -> hash_function)(key) % ht -> hmax;
	linked_list_t* bucket = ht -> buckets[index];
	ll_node_t* bucket_node = bucket -> head;

	if (!ht)
	{
		printf("Nonexistent hashtable!");
		return;
	}

	for (i = 0; i < bucket -> size; i++)
	{
		if (!(*ht -> compare_function)(key, ((info*)bucket_node -> data) -> key))
		{
			/* Deallocating all memory used by the entry */
			ll_remove_nth_node(bucket, i);
			free(((info*)bucket_node -> data) -> key);
			free(((info*)bucket_node -> data) -> value);
			free(bucket_node -> data);
			free(bucket_node);
			ht -> size--;
			return;
		}

		bucket_node = bucket_node -> next;
	}

	printf("Key not found!");
}

void
ht_free(hashtable_t *ht)
{
	unsigned i;

	if (!ht)
	{
		printf("Nonexistent hashtable!");
		return;
	}

	for (i = 0; i < ht -> hmax; i++)
	{
		linked_list_t* bucket = ht -> buckets[i];

		while(bucket -> head)
		{
			ht_remove_entry(ht, ((info*)bucket -> head -> data) -> key);
		}

		free(bucket);
	}

	free(ht -> buckets);
	free(ht);
}
