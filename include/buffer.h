#ifndef BUFFER_H
#define BUFFER_H

#include "my_types.h"
#include "utils.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>

class Buffer
{
private:
    uint8_t *buffer_begin;
    uint8_t *buffer_end;
    uint8_t *data_begin;
    uint8_t *data_end;

public:
    // Constructor & Destructor
    Buffer(size_t initial_capacity = 1024);
    ~Buffer();

    // Methods
    void append(const uint8_t *data, std::size_t len);
    void consume(std::size_t n);
    void resize(std::size_t new_size);
    uint8_t &operator[](std::size_t index);
    const uint8_t &operator[](std::size_t index) const;
    size_t size() const;
    uint8_t *data() const;
};

#endif
