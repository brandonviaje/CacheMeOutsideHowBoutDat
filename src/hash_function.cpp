#include "../include/hash_function.h"

namespace hash
{
    // rotate left for 64-bit ints
    static inline uint64_t rotate_left64(uint64_t val, int shift)
    {
        return (val << shift) | (val >> (64 - shift));
    }

    // load helpers
    static inline uint64_t load_uint64(const uint8_t *ptr)
    {
        uint64_t value{};
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }

    static inline uint32_t load_uint32(const uint8_t *ptr)
    {
        uint32_t value{};
        std::memcpy(&value, ptr, sizeof(value));
        return value;
    }

    // xxhash64 function
    [[nodiscard]] uint64_t xxhash64(const uint8_t *data, std::size_t len, uint64_t seed)
    {
        // xxhash constants
        constexpr uint64_t prime1{11400714785074694791ull};
        constexpr uint64_t prime2{14029467366897019727ull};
        constexpr uint64_t prime3{1609587929392839161ull};
        constexpr uint64_t prime4{9650029242287828579ull};
        constexpr uint64_t prime5{2870177450012600261ull};

        const uint8_t *start{data};
        const uint8_t *end{start + len};

        uint64_t hash_val{};

        // large input: use 4-lane accumulators
        if (len >= 32)
        {
            uint64_t accumulator1{seed + prime1 + prime2};
            uint64_t accumulator2{seed + prime2};
            uint64_t accumulator3{seed};
            uint64_t accumulator4{seed - prime1};

            // process 32 bytes at a time
            do
            {
                accumulator1 += load_uint64(start) * prime2;
                start += 8;
                accumulator1 = rotate_left64(accumulator1, 31) * prime1;

                accumulator2 += load_uint64(start) * prime2;
                start += 8;
                accumulator2 = rotate_left64(accumulator2, 31) * prime1;

                accumulator3 += load_uint64(start) * prime2;
                start += 8;
                accumulator3 = rotate_left64(accumulator3, 31) * prime1;

                accumulator4 += load_uint64(start) * prime2;
                start += 8;
                accumulator4 = rotate_left64(accumulator4, 31) * prime1;

            } while (start <= end - 32);

            // merge lanes into one hash value
            hash_val = rotate_left64(accumulator1, 1) + rotate_left64(accumulator2, 7) + rotate_left64(accumulator3, 12) + rotate_left64(accumulator4, 18);
        }
        else
        {
            // if len small, use simpler base hash
            hash_val = seed + prime5;
        }

        // mix in length
        hash_val += len;

        // process remaining 8-byte chunks
        while (start + 8 <= end)
        {
            uint64_t k1{load_uint64(start) * prime2};
            k1 = rotate_left64(k1, 31) * prime1;

            hash_val ^= k1;
            hash_val = rotate_left64(hash_val, 27) * prime1 + prime4;

            start += 8;
        }

        // process remaining 4 bytes
        if (start + 4 <= end)
        {
            uint64_t k1{load_uint32(start) * prime1};
            hash_val ^= k1;
            hash_val = rotate_left64(hash_val, 23) * prime2 + prime3;
            start += 4;
        }

        // process tail bytes
        while (start < end)
        {
            hash_val ^= (*start) * prime5;
            hash_val = rotate_left64(hash_val, 11) * prime1;
            ++start;
        }

        // final avalanche
        hash_val ^= hash_val >> 33;
        hash_val *= prime2;
        hash_val ^= hash_val >> 29;
        hash_val *= prime3;
        hash_val ^= hash_val >> 32;

        return hash_val;
    }

}
