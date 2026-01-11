#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include <cstdint>
void MurmurHash3_x64_128(const void *key, const int len, const uint32_t seed, void *out);

#endif
