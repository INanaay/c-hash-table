//
// Created by NANAA on 17/01/20.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_table.h"
#include "../include/prime.h"

static ht_item HT_DELETED_ITEM = {NULL, NULL};


static ht_hash_table* ht_new_sized(const int base_size) {
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));
	ht->base_size = base_size;

	ht->size = next_prime(ht->base_size);

	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
	return ht;
}

static ht_item *ht_new_item(const char *key, const char *value) {
	ht_item *new_item = malloc(sizeof(ht_item));
	new_item->key = strdup(key);
	new_item->value = strdup(value);

	return new_item;
}

ht_hash_table *ht_new() {
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}



static void ht_delete_item(ht_item *item) {
	free(item->value);
	free(item->key);
	free(item);
}


static void ht_resize(ht_hash_table* ht, const int base_size) {
	if (base_size < HT_INITIAL_BASE_SIZE) {
		return;
	}
	ht_hash_table* new_ht = ht_new_sized(base_size);
	for (int i = 0; i < ht->size; i++) {
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETED_ITEM) {
			ht_insert(new_ht, item->key, item->value);
		}
	}

	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	 ht_delete_hash_table(new_ht);
}


static void ht_resize_up(ht_hash_table* ht) {
	const int new_size = ht->base_size * 2;
	ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
	const int new_size = ht->base_size / 2;
	ht_resize(ht, new_size);
}

void ht_delete_hash_table(ht_hash_table *table) {
	for (int index = 0; index < table->size; index++) {
		ht_item *item = table->items[index];
		if (item) {
			ht_delete_item(item);
		}
	}

	free(table->items);
	free(table);
}

static int ht_hash(const char* s, const int a, const int m) {
	long hash = 0;
	const int len_s = strlen(s);
	for (int i = 0; i < len_s; i++) {
		hash += (long)pow(a, len_s - (i+1)) * s[i];
		hash = hash % m;
	}
	return (int)hash;
}

static int ht_get_hash(const char* s, const int num_buckets, const int attempt) {
	const int hash_a = ht_hash(s, HT_PRIME1, num_buckets);
	const int hash_b = ht_hash(s, HT_PRIME2, num_buckets);
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

int ht_insert(ht_hash_table *table, const char *key, const char *value) {
	int count = 1;

	const int load = table->count * 100 / table->size;
	if (load > 70) {
		ht_resize_up(table);
	}

	ht_item *new_item = ht_new_item(key, value);
	if (!new_item)
		return 1;
	int index = ht_get_hash(new_item->key, table->size, 0);
	while (table->items[index]) {
		if (table->items[index] != &HT_DELETED_ITEM) {
			if (strcmp(table->items[index]->key, key) == 0) {
				ht_delete_item(table->items[index]);
				table->items[index] = new_item;
			}
		}

		index = ht_get_hash(new_item->key, table->size, count);
		count++;
	}
	table->items[index] = new_item;
	table->count++;
	return 0;
}



char *ht_search(ht_hash_table *table, char *key) {
	int index = ht_get_hash(key, table->size, 0);

	int count = 1;
	ht_item *item = table->items[index];
	while (item != NULL) {
		if (item != &HT_DELETED_ITEM) {
			if (strcmp(item->key, key) == 0)
				return item->value;
			index = ht_get_hash(key, table->size, count);
			item = table->items[index];
			count++;
		}
	}
	return NULL;
}

/**The item we wish to delete might be part of a collision chain. If we remove it, it will break the chain
 * We just mark the item as deleted.
 */

void ht_delete(ht_hash_table *table, const char *key) {

	const int load = table->count * 100 / table->size;
	if (load < 10) {
		ht_resize_down(table);
	}

	int index = ht_get_hash(key, table->size, 0);
	int count = 1;

	ht_item *item = table->items[index];
	while (item != NULL) {
		if (item != &HT_DELETED_ITEM) {
			if (strcmp(item->key, key) == 0) {
				ht_delete_item(item);
				table->items[index] = &HT_DELETED_ITEM;
			}
		}
		index = ht_get_hash(key, table->size, count);
		item = table->items[index];
		count++;
	}
	table->count--;
}