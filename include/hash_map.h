#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "hash_table.h"

const std::size_t MAX_LOAD_FACTOR = 8;
const std::size_t REHASHING_WORK = 128;

struct HashMap
{
    HashTable newer;
    HashTable older;
    std::size_t migrate_position{};
};

void hashmap_migrate_keys(HashMap* hashmap);
void hashmap_trigger_rehashing(HashMap* hashmap);
HashNode* hashmap_lookup(HashMap *hashmap, HashNode *key, bool (*eq)(HashNode *, HashNode *));
void hashmap_insert(HashMap *hashmap, HashNode *node);
HashNode* hashmap_delete(HashMap *hashmap, HashNode *key, bool (*eq)(HashNode *, HashNode *));

#endif
