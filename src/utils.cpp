#include "../include/utils.h"

void msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

void set_nonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int32_t print_response(const uint8_t *data, size_t size)
{
    if (size < 1)
    {
        msg("bad response");
        return -1;
    }

    switch (static_cast<Tag>(data[0]))
    {
    case Tag::TAG_NIL:

        printf("(nil)\n");
        return 1;

    case Tag::TAG_ERR:

        if (size < 1 + 8)
        {
            msg("bad response");
            return -1;
        }
        {
            int32_t code{};
            uint32_t len{};
            memcpy(&code, &data[1], 4);
            memcpy(&len, &data[1 + 4], 4);

            if (size < 1 + 8 + len)
            {
                msg("bad response");
                return -1;
            }

            printf("(err) %d %.*s\n", code, len, &data[1 + 8]);
            return 1 + 8 + len;
        }

    case Tag::TAG_STR:

        if (size < 1 + 4)
        {
            msg("bad response");
            return -1;
        }
        {
            uint32_t len = 0;
            memcpy(&len, &data[1], 4);
            if (size < 1 + 4 + len)
            {
                msg("bad response");
                return -1;
            }
            printf("(str) %.*s\n", len, &data[1 + 4]);
            return 1 + 4 + len;
        }

    case Tag::TAG_INT:

        if (size < 1 + 8)
        {
            msg("bad response");
            return -1;
        }
        {
            int64_t val = 0;
            memcpy(&val, &data[1], 8);
            printf("(int) %ld\n", val);
            return 1 + 8;
        }

    case Tag::TAG_DBL:

        if (size < 1 + 8)
        {
            msg("bad response");
            return -1;
        }
        {
            double val = 0;
            memcpy(&val, &data[1], 8);
            printf("(dbl) %g\n", val);
            return 1 + 8;
        }

    case Tag::TAG_ARR:

        if (size < 1 + 4)
        {
            msg("bad response");
            return -1;
        }
        {
            uint32_t len = 0;
            memcpy(&len, &data[1], 4);
            printf("(arr) len=%u\n", len);
            size_t arr_bytes = 1 + 4;
            for (uint32_t i = 0; i < len; ++i)
            {
                int32_t rv = print_response(&data[arr_bytes], size - arr_bytes);
                if (rv < 0)
                {
                    return rv;
                }
                arr_bytes += (size_t)rv;
            }
            printf("(arr) end\n");
            return (int32_t)arr_bytes;
        }
    default:
        msg("bad response");
        return -1;
    }
}

int32_t read_full(int fd, void *buffer, ssize_t total_bytes)
{
    uint8_t *buf{static_cast<uint8_t *>(buffer)};

    while (total_bytes > 0)
    {
        ssize_t bytes_read = recv(fd, buf, total_bytes, 0);
        if (bytes_read == -1)
        {
            std::cerr << "recv failed: " << strerror(errno) << '\n';
            return bytes_read;
        }
        else if (bytes_read == 0)
        {
            std::cerr << "EOF\n";
            return -1;
        }

        assert(bytes_read <= total_bytes);
        total_bytes -= bytes_read;
        buf += bytes_read;
    }

    return 0;
}

int32_t write_all(int fd, const void *buffer, ssize_t total_bytes)
{
    const uint8_t *buf{static_cast<const uint8_t *>(buffer)};

    while (total_bytes > 0)
    {
        ssize_t bytes_sent{send(fd, buf, total_bytes, 0)};
        if (bytes_sent == -1)
        {
            std::cerr << "send() failed: " << strerror(errno) << "\n";
            return -1;
        }

        assert(bytes_sent <= total_bytes);
        total_bytes -= bytes_sent;
        buf += bytes_sent;
    }

    return 0;
}

int32_t send_request(int fd, const std::vector<std::string> &cmd)
{
    Buffer write_buffer;

    // number of strings
    uint32_t nstr = htonl(static_cast<uint32_t>(cmd.size()));
    write_buffer.append(reinterpret_cast<uint8_t *>(&nstr), 4);

    // each string: 4-byte length + string data
    for (const auto &s : cmd)
    {
        uint32_t slen = htonl(static_cast<uint32_t>(s.size()));
        write_buffer.append(reinterpret_cast<uint8_t *>(&slen), 4);
        write_buffer.append(reinterpret_cast<const uint8_t *>(s.data()), s.size());
    }

    // prepend total length (of remaining message) for your protocol
    uint32_t total_len = htonl(static_cast<uint32_t>(write_buffer.size()));
    Buffer final_buffer;
    final_buffer.append(reinterpret_cast<uint8_t *>(&total_len), 4);
    final_buffer.append(write_buffer.data(), write_buffer.size());

    return write_all(fd, final_buffer.data(), final_buffer.size());
}

int32_t read_result(int fd)
{
    std::vector<uint8_t> read_buffer(4);
    if (read_full(fd, &read_buffer[0], 4) != 0)
        return -1;

    uint32_t len{};
    std::memcpy(&len, read_buffer.data(), 4);
    len = ntohl(len);

    if (len > MAX_MSG_SIZE)
    {
        msg("Response too big");
        return -1;
    }

    read_buffer.resize(4 + len);

    if (read_full(fd, &read_buffer[4], len) != 0)
        return -1;

    int32_t status = print_response(&read_buffer[4], len); // TLV parser

    if (status > 0 && (uint32_t)status != len)
    {
        msg("bad response");
        return -1;
    }
    return 0;
}

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
    buffer_append_u32(out, static_cast<uint32_t>(msg.size()));
    buffer_append(out, reinterpret_cast<const uint8_t *>(msg.data()), msg.size());
}

bool read_u32(const uint8_t *&cur, const uint8_t *end, uint32_t &out)
{
    if (cur + 4 > end)
        return false;
    std::memcpy(&out, cur, 4);
    out = ntohl(out);
    cur += 4;
    return true;
}

bool read_str(const uint8_t *&cur, const uint8_t *end, size_t n, std::string &out)
{
    if (cur + n > end)
    {
        return false;
    }
    out.assign(reinterpret_cast<const char *>(cur), n);
    cur += n;
    return true;
}