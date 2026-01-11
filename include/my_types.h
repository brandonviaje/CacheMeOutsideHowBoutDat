#ifndef MY_TYPES
#define MY_TYPES

#define MAXCONN 4096
#define PORT 8080

#include <iostream>
#include <vector>
#include <cstddef>

enum class Tag
{
    TAG_NIL = 0,
    TAG_ERR = 1,
    TAG_STR = 2,
    TAG_INT = 3,
    TAG_DBL = 4,
    TAG_ARR = 5,
};

enum class Error
{
    ERR_UNKNOWN = 1,
    ERR_TOO_BIG = 2,
};

constexpr size_t MAX_MSG_SIZE = (32 << 20);

#endif
