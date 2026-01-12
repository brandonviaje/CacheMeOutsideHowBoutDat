#include "../include/utils.h"

void msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

void set_nonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
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

int32_t send_request(int fd, const uint8_t *text, size_t len)
{
    if (len > MAX_MSG_SIZE)
    {
        return -1;
    }

    Buffer write_buffer;

    // add message length and data
    write_buffer.append(reinterpret_cast<const uint8_t *>(&len), 4);
    write_buffer.append(text, len);

    // write bytes to socket
    int32_t written{write_all(fd, write_buffer.data(), write_buffer.size())};

    return written;
}

int32_t read_result(int fd)
{
    // header
    std::vector<uint8_t> read_buffer;
    read_buffer.resize(4);
    errno = 0;

    int32_t err{read_full(fd, &read_buffer[0], 4)};

    if (err)
    {
        if (errno == 0)
        {
            msg("EOF");
        }
        else
        {
            msg("read() error");
        }
        return err;
    }

    uint32_t len{};
    std::memcpy(&len, read_buffer.data(), 4);

    if (len > MAX_MSG_SIZE)
    {
        msg("too long");
        return -1;
    }

    // reply body
    read_buffer.resize(4 + len);
    err = read_full(fd, &read_buffer[4], len);

    if (err)
    {
        msg("read() error");
        return err;
    }

    // TODO: do something
    printf("len:%u data:%.*s\n", len, len < 100 ? len : 100, &read_buffer[4]);
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
    buffer_append_u32(out, static_cast<uint8_t>(msg.size()));
    buffer_append(out, reinterpret_cast<const uint8_t *>(msg.data()), msg.size());
}

bool read_u32(const uint8_t *&cur, const uint8_t *end, uint32_t &out)
{
    if (cur + 4 > end)
    {
        return false;
    }
    memcpy(&out, cur, 4);
    cur += 4;
    return true;
}

bool read_str(const uint8_t *&cur, const uint8_t *end, size_t n, std::string &out)
{
    if (cur + n > end)
    {
        return false;
    }
    out.assign(cur, cur + n);
    cur += n;
    return true;
}