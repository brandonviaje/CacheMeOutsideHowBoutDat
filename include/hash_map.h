#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "my_types.h"
#include "hash_table.h"
#include "hash_function.h"

const std::size_t MAX_LOAD_FACTOR = 8;
const std::size_t REHASHING_WORK = 128;

struct HashMap
{
    HashTable newer;
    HashTable older;
    std::size_t migrate_position{};
};

struct Entry
{
    HashNode node;
    std::string key;
    std::string val;
};

void hashmap_migrate_keys(HashMap *hashmap);
void hashmap_trigger_rehashing(HashMap *hashmap);
HashNode *hashmap_lookup(HashMap *hashmap, HashNode *key, bool (*eq)(HashNode *, HashNode *));
void hashmap_insert(HashMap *hashmap, HashNode *node);
std::size_t hashmap_size(HashMap *hashmap);
HashNode *hashmap_delete(HashMap *hashmap, HashNode *key, bool (*eq)(HashNode *, HashNode *));
bool hashtable_foreach(HashTable *hashtable, bool (*f)(HashNode *, void *), void *arg);
bool hashmap_foreach(HashMap *hashmap, bool (*f)(HashNode *, void *), void *arg);
bool entry_eq(HashNode *lhs, HashNode *rhs);

#endif
