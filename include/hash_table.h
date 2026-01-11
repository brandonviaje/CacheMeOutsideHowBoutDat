#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <cstddef>
#include <cstdint>
#include <assert.h>
#include <stdlib.h>

struct HashNode
{
    HashNode *next{};
    uint64_t hash_code{};
};

struct HashTable
{
    HashNode **table{};
    std::size_t mask{};
    std::size_t size{};
};

void hash_table_init(HashTable *hashtable, std::size_t n);
void hash_node_insert(HashTable *hashtable, HashNode *node);
HashNode **hash_node_lookup(HashTable *hashtable, HashNode *key, bool (*eq)(HashNode *, HashNode *));
HashNode *hash_node_delete(HashTable *hashtable, HashNode **from);

#endif
