#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace hash
{
    [[nodiscard]] uint64_t xxhash64(const uint8_t *data, std::size_t len, uint64_t seed = 0);
}

#endif
