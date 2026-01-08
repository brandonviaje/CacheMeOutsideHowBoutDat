#include "buffer.h"

Buffer::Buffer(size_t initial_capacity) {
    buffer_begin = (uint8_t*)malloc(initial_capacity);
    buffer_end   = buffer_begin + initial_capacity;
    data_begin   = buffer_begin;
    data_end     = buffer_begin;
}

Buffer::~Buffer() {
    free(buffer_begin);
    buffer_begin = buffer_end = data_begin = data_end = nullptr;
}

void Buffer::append(const uint8_t* data, size_t len) {
    size_t used {static_cast<size_t>(data_end - buffer_begin)};
    size_t capacity {static_cast<size_t>(buffer_end - buffer_begin)};

    if (used + len > capacity) 
    {
        size_t new_capacity {capacity ? capacity * 2 : 1024};

        while (new_capacity < used + len) 
        {
            new_capacity *= 2;
        }

        uint8_t* new_buffer {static_cast<uint8_t *>(malloc(new_capacity))};
        size_t data_len {static_cast<size_t>(data_end - data_begin)};

        if (data_len > 0) 
        {
            memcpy(new_buffer, data_begin, data_len);
        }

        free(buffer_begin);

        buffer_begin = new_buffer;
        buffer_end   = new_buffer + new_capacity;
        data_begin   = new_buffer;
        data_end     = new_buffer + data_len;
    }

    memcpy(data_end, data, len);
    data_end += len;
}

void Buffer::consume(size_t n) 
{
    data_begin += n;
    if (data_begin == data_end) 
    {
        data_begin = buffer_begin;
        data_end = buffer_begin;
    }
}

size_t Buffer::size() const 
{
    return static_cast<size_t>(data_end - data_begin);
}

uint8_t* Buffer::data() const 
{
    return data_begin;
}
