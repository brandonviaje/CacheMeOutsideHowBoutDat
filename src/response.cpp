#include "../include/reponse.h"

// helper functions for appending TLV to buffer
void buffer_append_u8(Buffer &buffer, uint8_t data)
{
    buffer.append(&data, 1);
}

void buffer_append_u32(Buffer &buffer, uint32_t data)
{
    buffer.append(reinterpret_cast<uint8_t *>(&data), 4);
}

void buffer_append_i64(Buffer &buffer, int64_t data)
{
    buffer.append(reinterpret_cast<uint8_t *>(&data), 8);
}

void buffer_append_f64(Buffer &buffer, double data)
{
    buffer.append(reinterpret_cast<uint8_t *>(&data), 8);
}

void buffer_append(Buffer &buffer, const uint8_t *data, std::size_t n)
{
    buffer.append(data, n);
}

// Output serialized data using TLV format
//
// Tag: 1 byte (buffer_append_u8)
// Len: 4 bytes (buffer_append_u32), only for variable-length values (e.g., strings, arrays)
// Val: actual data
//
// - For fixed-size types (int64_t, double, uint8_t, etc), length is implicit and not included
// - For variable-size types (string, arrays), length field is included before the value

void output_nil(Buffer &out)
{
    // tag
    buffer_append_u8(out, static_cast<uint8_t>(Tag::TAG_NIL));
}

void output_str(Buffer &out, const std::string &s)
{
    // tag + len + val
    buffer_append_u8(out, static_cast<uint8_t>(Tag::TAG_STR));
    buffer_append_u32(out, static_cast<uint32_t>(s.size()));
    buffer_append(out, reinterpret_cast<const uint8_t *>(s.data()), s.size());
}

void output_int(Buffer &out, int64_t val)
{
    // tag + val (size is implicit)
    buffer_append_u8(out, static_cast<uint8_t>(Tag::TAG_INT));
    buffer_append_i64(out, val);
}

void output_arr(Buffer &out, uint32_t n)
{
    // tag + len
    buffer_append_u8(out, static_cast<uint8_t>(Tag::TAG_ARR));
    buffer_append_u32(out, n);
}

void output_dbl(Buffer &out, double val)
{
    // tag + val (size is implicit)
    buffer_append_u8(out, static_cast<uint8_t>(Tag::TAG_DBL));
    buffer_append_f64(out, val);
}

void output_err(Buffer &out, uint32_t code, const std::string &msg)
{
    // tag + error code +
    buffer_append_u8(out, static_cast<uint8_t>(Tag::TAG_ERR));
    buffer_append_u32(out, code);
    buffer_append_u32(out, static_cast<uint8_t>(msg.size()));
    buffer_append(out, reinterpret_cast<const uint8_t *>(msg.data()), msg.size());
}

void response_begin(Buffer &out, std::size_t *header)
{
    *header = out.size();
    buffer_append_u32(out, *header);
}

std::size_t response_size(Buffer &out, std::size_t header)
{
    return out.size() - header - 4;
}

void response_end(Buffer *out, std::size_t header)
{
    std::size_t msg_size{response_size(*out, header)};

    if (msg_size > MAX_MSG_SIZE)
    {
        out->resize(header + 4);
        output_err(*out, static_cast<uint32_t>(Error::ERR_TOO_BIG), "Message is Too Long.");
        msg_size = response_size(*out, header);
    }

    uint32_t len{msg_size};
    std::memcpy(&out[header], &len, 4);
}