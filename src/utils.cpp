#include "utils.h"

void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

void buffer_append(std::vector<uint8_t> &buffer, const uint8_t *data, size_t len)
{
    buffer.insert(buffer.end(), data, data+len);
}

void buffer_consume(std::vector<uint8_t> &buffer, size_t n)
{
    buffer.erase(buffer.begin(), buffer.begin() + n);
}

void set_nonblocking(int fd) 
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int32_t read_full(int fd, void* buffer, ssize_t total_bytes)
{
    uint8_t* buf {static_cast<uint8_t*>(buffer)}; 

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

int32_t write_all(int fd, const void* buffer, ssize_t total_bytes)
{
    const uint8_t* buf {static_cast<const uint8_t*>(buffer)};

    while (total_bytes > 0)
    {
        ssize_t bytes_sent {send(fd, buf, total_bytes, 0)};
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

    std::vector<uint8_t> wbuf;
    buffer_append(wbuf, (const uint8_t *)&len, 4);
    buffer_append(wbuf, text, len);
    return write_all(fd, wbuf.data(), wbuf.size());
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

    uint32_t len = 0;
    memcpy(&len, read_buffer.data(), 4);  

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
