#include "../include/hash_map.h"

void hashmap_migrate_keys(HashMap *hashmap)
{
    std::size_t nwork{}; // keep track of num of nodes moved so far

    while (nwork < REHASHING_WORK && hashmap->older.size > 0)
    {
        // find an occupied slot
        HashNode **from{&hashmap->older.table[hashmap->migrate_position]};

        if (!*from)
        {
            hashmap->migrate_position++;
            continue; // empty slot, continue
        }

        // move first node from older to newer table, update num of nodes moved
        hash_node_insert(&hashmap->newer, hash_node_delete(&hashmap->older, from));
        nwork++;
    }

    // delete old table if migration done
    if (hashmap->older.size == 0 && hashmap->older.table)
    {
        free(hashmap->older.table);
        hashmap->older = HashTable{};
    }
}

void hashmap_trigger_rehashing(HashMap *hashmap)
{
    hashmap->older = hashmap->newer;                                 // older table now the prev new table
    hash_table_init(&hashmap->newer, (hashmap->newer.mask + 1) * 2); // double size for new table
    hashmap->migrate_position = 0;                                   // reset migration index
}

// GET,SET,DEL interfaces

HashNode *hashmap_lookup(HashMap *hashmap, HashNode *key, bool (*eq)(HashNode *, HashNode *))
{
    // query newer table first
    HashNode **from{hash_node_lookup(&hashmap->newer, key, eq)};

    if (from && *from)
    {
        return *from;
    }

    // query older table if not found in newer table
    from = hash_node_lookup(&hashmap->older, key, eq);

    if (from && *from)
    {
        return *from;
    }

    return nullptr; // not found
}

void hashmap_insert(HashMap *hashmap, HashNode *node)
{

    // init newer table if empty
    if (!hashmap->newer.table)
    {
        hash_table_init(&hashmap->newer, 4);
    }

    hash_node_insert(&hashmap->newer, node); // insertion should update the newer table

    // check if we need to rehash (load factor is high)
    if (!hashmap->older.table)
    {
        std::size_t threshold{(hashmap->newer.mask + 1) * MAX_LOAD_FACTOR};

        if (hashmap->newer.size >= threshold)
        {
            hashmap_trigger_rehashing(hashmap);
        }
    }

    // migrate keys
    hashmap_migrate_keys(hashmap);
}

HashNode *hashmap_delete(HashMap *hashmap, HashNode *key, bool (*eq)(HashNode *, HashNode *))
{
    // query newer table first
    HashNode **from{hash_node_lookup(&hashmap->newer, key, eq)};

    if (from && *from)
    {
        return hash_node_delete(&hashmap->newer, from);
    }

    // query older table next if not found in newer table
    from = hash_node_lookup(&hashmap->older, key, eq);

    if (from && *from)
    {
        return hash_node_delete(&hashmap->older, from);
    }

    return nullptr; // not found
}

std::size_t hashmap_size(HashMap *hashmap)
{
    return hashmap->newer.size + hashmap->older.size;
}

bool hashtable_foreach(HashTable *hashtable, bool (*f)(HashNode *, void *), void *arg)
{
    for (std::size_t i{}; hashtable->mask != 0 && i <= hashtable->mask; i++)
    {
        for (HashNode *node{hashtable->table[i]}; node != NULL; node = node->next)
        {
            if (!f(node, arg))
                return false;
        }
    }
    return true;
}

bool entry_eq(HashNode *lhs, HashNode *rhs)
{
    Entry *le = container_of(lhs, struct Entry, node);
    Entry *re = container_of(rhs, struct Entry, node);
    return le->key == re->key;
}

bool hashmap_foreach(HashMap *hashmap, bool (*f)(HashNode *, void *), void *arg)
{
    hashtable_foreach(&hashmap->newer, f, arg) && hashtable_foreach(&hashmap->older, f, arg);
}
