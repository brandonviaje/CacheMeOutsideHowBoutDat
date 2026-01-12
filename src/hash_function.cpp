#include "../include/hash_function.h"

uint64_t str_hash(const uint8_t *data, std::size_t len)
{
    uint32_t hash{0x811C9DC5};
    for (std::size_t i = 0; i < len; i++)
    {
        hash = (hash + data[i]) * 0x01000193;
    }
    return hash;
}
