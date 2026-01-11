#include "../include/buffer.h"

Buffer::Buffer(size_t initial_capacity)
{
    buffer_begin = static_cast<uint8_t *>(malloc(initial_capacity));

    if (!buffer_begin)
    {
        throw std::bad_alloc();
    }

    buffer_end = buffer_begin + initial_capacity;
    data_begin = buffer_begin;
    data_end = buffer_begin;
}

Buffer::~Buffer()
{
    free(buffer_begin);
    buffer_begin = buffer_end = data_begin = data_end = nullptr;
}

void Buffer::append(const uint8_t *data, size_t len)
{
    if (!data)
        return;

    size_t used{static_cast<size_t>(data_end - buffer_begin)};
    size_t capacity{static_cast<size_t>(buffer_end - buffer_begin)};

    // resize if needed
    if (used + len > capacity)
    {
        size_t new_capacity{capacity ? capacity * 2 : 1024};

        while (new_capacity < used + len)
        {
            new_capacity *= 2;
        }

        uint8_t *new_buffer{static_cast<uint8_t *>(malloc(new_capacity))};
        size_t data_len{static_cast<size_t>(data_end - data_begin)};

        if (data_len > 0)
        {
            memcpy(new_buffer, data_begin, data_len);
        }

        free(buffer_begin);

        // update buffer fields
        buffer_begin = new_buffer;
        buffer_end = new_buffer + new_capacity;
        data_begin = new_buffer;
        data_end = new_buffer + data_len;
    }

    memcpy(data_end, data, len);
    data_end += len;
}

void Buffer::consume(size_t n)
{
    if (n > size())
    {
        throw std::out_of_range("consume more than buffer size");
    }

    data_begin += n;

    if (data_begin == data_end)
    {
        data_begin = buffer_begin;
        data_end = buffer_begin;
    }
}

void Buffer::resize(size_t new_size)
{
    size_t current_size{size()};

    if (new_size > static_cast<size_t>(buffer_end - buffer_begin))
    {
        uint8_t *new_buffer{static_cast<uint8_t *>(malloc(new_size))};

        if (!new_buffer)
        {
            throw std::bad_alloc();
        }

        if (current_size > 0)
        {
            memcpy(new_buffer, data_begin, current_size);
        }

        free(buffer_begin);
        buffer_begin = new_buffer;
        buffer_end = new_buffer + new_size;
        data_begin = new_buffer;
        data_end = new_buffer + current_size;
    }

    else
    {
        // shrink only, never extend data beyond current size
        if (new_size < current_size)
        {
            data_end = data_begin + new_size;
        }
    }
}

uint8_t &Buffer::operator[](size_t index)
{
    if (index >= size())
    {
        throw std::out_of_range("Buffer index out of range");
    }
    return data_begin[index];
}

const uint8_t &Buffer::operator[](size_t index) const
{
    if (index >= size())
    {
        throw std::out_of_range("Buffer index out of range");
    }
    return data_begin[index];
}

size_t Buffer::size() const
{
    return static_cast<size_t>(data_end - data_begin);
}

uint8_t *Buffer::data() const
{
    return data_begin;
}