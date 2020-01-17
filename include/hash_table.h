//
// Created by NANAA on 17/01/20.
//

#ifndef C_HASH_TABLE_HASH_TABLE_H
#define C_HASH_TABLE_HASH_TABLE_H

#define HT_PRIME1 151
#define HT_PRIME2 157

typedef struct {
	char *key;
	char *value;
} ht_item;

typedef struct {
	int size;
	int count;
	ht_item **items;
} ht_hash_table;


void ht_delete_hash_table(ht_hash_table *table);
ht_hash_table *ht_new();

#endif //C_HASH_TABLE_HASH_TABLE_H
