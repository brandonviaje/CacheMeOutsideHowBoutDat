#include "../include/hash_table.h"

// init hash table with n buckets
void hash_table_init(HashTable *hashtable, std::size_t n)
{
    assert(n > 0 && ((n - 1) & n) == 0); // make sure n is a power of 2 for fast masking

    hashtable->table = new HashNode *[n](); // allocate n bucket slots
    hashtable->mask = n - 1;                // mask = n-1 lets us do hash & mask bitmask instead of modulo operator (slow)
    hashtable->size = 0;                    // start with zero elements
}

// insert node into table
void hash_node_insert(HashTable *hashtable, HashNode *node)
{
    // get which bucket
    std::size_t position = node->hash_code & hashtable->mask;

    // insert at the front of the linked list
    HashNode *next{hashtable->table[position]};
    node->next = next;
    hashtable->table[position] = node;

    hashtable->size++; // update table size
}

// lookup node in table
HashNode **hash_node_lookup(HashTable *hashtable, HashNode *key, bool (*eq)(HashNode *, HashNode *))
{
    // if table isn’t init, return
    if (!hashtable->table)
    {
        return nullptr;
    }

    // get bucket
    std::size_t position{key->hash_code & hashtable->mask};
    HashNode **from{&hashtable->table[position]};

    // iterate through linked list
    for (HashNode *current; (current = *from) != NULL; from = &current->next)
    {
        // if hash matches and eq says yes, we found node
        if (current->hash_code == key->hash_code && eq(current, key))
        {
            return from; // return pointer-to-pointer so we can delete/modify
        }
    }

    // else, didn’t find it, return insertion point
    return from;
}

// delete node from table
HashNode *hash_node_delete(HashTable *hashtable, HashNode **from)
{
    // safety check
    if (!from || !*from)
    {
        return nullptr;
    }

    HashNode *node{*from};
    *from = node->next;   // unlink from list
    node->next = nullptr; // clear node’s next
    hashtable->size--;    // update table size

    return node; // return deleted node
}
