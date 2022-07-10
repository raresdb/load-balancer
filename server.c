#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "Hashtable.h"
#include "utils.h"

server_memory* init_server_memory() {
	server_memory* server = malloc(sizeof(server_memory));
	DIE(!server, "Error when allocating server memory!");

	server -> data = ht_create(MAX_BUCKET_SIZE, hash_function_string,
							   compare_function_strings);
	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server -> data, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server -> data, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return (char*)ht_get(server -> data, key);
}

void free_server_memory(server_memory* server) {
	ht_free(server -> data);
	free(server);
}
