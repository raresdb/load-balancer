#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "LinkedList.h"

typedef struct {
	void *key;
	void *value;
} info;

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Simply linked lists array */
	unsigned int size; /* Number of current entries */
	unsigned int hmax; /* Number of buckets */
	/* Hashing function given through pointer */
	unsigned int (*hash_function)(void*);
	/* Comparing function for keys given through pointer */
	int (*compare_function)(void*, void*);
};

/* Function used for memory allocation and initialisation of a hashtable*/
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));

/* Function used for introducing a new entry into the hashtable */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);

/* Function used for receiving the value associated with a given key */
void *
ht_get(hashtable_t *ht, void *key);

/* Function for checking whether a given key has been added to the hashtable */
int
ht_has_key(hashtable_t *ht, void *key);

/* Function used for removing an entry from the hashtable */
void
ht_remove_entry(hashtable_t *ht, void *key);

/* Function used for deallocating all the memory occupied by the hashtable */
void
ht_free(hashtable_t *ht);

#endif /* HASHTABLE_H_ */
