#include "hash-table-base.h"

#include <errno.h>
#include <assert.h>
#include <stdio.h> //needed for perror
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include <pthread.h>

struct list_entry {
	const char *key;
	uint32_t value;
	SLIST_ENTRY(list_entry) pointers;
};

SLIST_HEAD(list_head, list_entry);

struct hash_table_entry {
	struct list_head list_head;
};

struct hash_table_v2 {
	struct hash_table_entry entries[HASH_TABLE_CAPACITY];
};

struct hash_table_v2 *hash_table_v2_create()
{
	struct hash_table_v2 *hash_table = calloc(1, sizeof(struct hash_table_v2));
	assert(hash_table != NULL);
	for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i) {
		struct hash_table_entry *entry = &hash_table->entries[i];
		SLIST_INIT(&entry->list_head);
	}
	return hash_table;
}

static struct hash_table_entry *get_hash_table_entry(struct hash_table_v2 *hash_table,
                                                     const char *key)
{
	assert(key != NULL);
	uint32_t index = bernstein_hash(key) % HASH_TABLE_CAPACITY;
	struct hash_table_entry *entry = &hash_table->entries[index];
	return entry;
}

static struct list_entry *get_list_entry(struct hash_table_v2 *hash_table,
                                         const char *key,
                                         struct list_head *list_head)
{
	assert(key != NULL);

	struct list_entry *entry = NULL;
	
	SLIST_FOREACH(entry, list_head, pointers) {
	  if (strcmp(entry->key, key) == 0) {
	    return entry;
	  }
	}
	return NULL;
}

bool hash_table_v2_contains(struct hash_table_v2 *hash_table,
                            const char *key)
{
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	return list_entry != NULL;
}

void hash_table_v2_add_entry(struct hash_table_v2 *hash_table,
                             const char *key,
                             uint32_t value)
{
	static pthread_mutex_t mutex1;
	static pthread_mutex_t mutex2;
	if (pthread_mutex_init(&mutex1, NULL) != 0)
	{
		int err = errno;
		perror("init");
		exit(err);
	}
	if (pthread_mutex_init(&mutex2, NULL) != 0)
	{
		int err = errno;
		perror("init");
		exit(err);
	}
	//3 param: hash table, key-value pair
	//pass in hash table as well as key: given table, take in key, pass through hash func, get index into hash table corresponding to key
	//DONT LOCK HERE B/C JUST GETTING INDEX OF HASH TABLE ITSELF
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	//get notion of given that bucket, get buckets head (start of linked list)
	//LOCK HERE B/C 2 THREADS OF SAME BUCKET, MAY POINT TO SAME ONE
	if (pthread_mutex_lock(&mutex1) != 0)
	{
		int err = errno;
		perror("lock");
		exit(err);
	}
	struct list_head *list_head = &hash_table_entry->list_head;
	//get list entry, prop of hash table: no 2 elements can have same key, to prevent 2 nodes having same key, call this
	//hash table key and head, given bucket that we were indexed into, do linear scan in that buckets LL, and check that key trying to insert doesnt exist in LL
	//LOCK HERE FOR CASE THAT 2 WITH SAME KEY WILL THINK KEY DOES NOT EXIST YET
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	if(pthread_mutex_unlock(&mutex1) != 0)
	{
		int err = errno;
		perror("unlockv2L1");
		exit(err);
	}
	/* Update the value if it already exists */
	// 2 of same key cant exist
	//insetad of creating new node, we find node through get list entry, override value to value we are trying to insert
	//exit out of function as soon as we do that
	if (list_entry != NULL) {
		list_entry->value = value;
		return;
	}
	//if list entry is null, creates a new node for new entry
	//no same key, allocate memory for new ndode
	//assign key and value, insert into LL now
	list_entry = calloc(1, sizeof(struct list_entry));
	list_entry->key = key;
	list_entry->value = value;
	//LOCK HERE FOR CASE OF POSSIBLE INSERTION TO SAME HEAD
	if (pthread_mutex_lock(&mutex2) != 0)
	{
		int err = errno;
		perror("lock");
		exit(err);
	}
	SLIST_INSERT_HEAD(list_head, list_entry, pointers);
	if(pthread_mutex_unlock(&mutex2) != 0)
	{
		int err = errno;
		perror("unlockv2L2");
		exit(err);
	}
	if (pthread_mutex_destroy(&mutex1) != 0)
	{
		int err = errno;
		perror("destroy");
		exit(err);
	}
	if (pthread_mutex_destroy(&mutex2) != 0)
	{
		int err = errno;
		perror("destroy");
		exit(err);
	}
}

uint32_t hash_table_v2_get_value(struct hash_table_v2 *hash_table,
                                 const char *key)
{
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	assert(list_entry != NULL);
	return list_entry->value;
}

void hash_table_v2_destroy(struct hash_table_v2 *hash_table)
{
	for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i) {
		struct hash_table_entry *entry = &hash_table->entries[i];
		struct list_head *list_head = &entry->list_head;
		struct list_entry *list_entry = NULL;
		while (!SLIST_EMPTY(list_head)) {
			list_entry = SLIST_FIRST(list_head);
			SLIST_REMOVE_HEAD(list_head, pointers);
			free(list_entry);
		}
	}
	free(hash_table);
}
