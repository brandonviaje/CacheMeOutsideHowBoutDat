#include "utils.h"


void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

int32_t read_full(int fd, char* buffer, ssize_t total_bytes)
{
    // keep reading until we've got all the bytes we need
    while(total_bytes > 0)
    {
        ssize_t bytes_read { recv(fd, buffer, total_bytes, 0) };          // try to read from socket

        if (bytes_read == -1)
        {
            std::cerr << "recv failed: " << strerror(errno) << '\n';    // network oops
            return bytes_read;                                          // tell caller something went wrong
        }
        else if (bytes_read == 0)
        {
            std::cerr << "EOF\n";                                       // peer closed connection, no more data
            return -1; 
        }

        assert(bytes_read <= total_bytes);                              // make sure we didn’t read too much
        total_bytes -= bytes_read;                                      // decrease remaining bytes
        buffer += bytes_read;                                           // advance buffer pointer
    }

    return 0; 
}

int32_t write_all(int fd, const char* buffer, ssize_t total_bytes)
{
    // keep sending until all bytes go out
    while(total_bytes > 0)
    {
        ssize_t bytes_sent { send(fd, buffer, total_bytes, 0) };          // send to socket

        if(bytes_sent == -1)
        {
            std::cerr << "send() failed: " << strerror(errno) << "\n";  // send error
            return -1;
        }

        assert(bytes_sent <= total_bytes);                              // make sure we didn’t read too much
        total_bytes -= bytes_sent;                                      // decrease remaining bytes
        buffer += bytes_sent;                                           // advance buffer pointer
    }

    return 0; 
}

int32_t query(int fd, const char *text) {
    uint32_t len = strlen(text);
    char wbuf[4 + MAXCONN];

    // convert header to network byte order
    uint32_t net_len = htonl(len);
    memcpy(wbuf, &net_len, 4);
    memcpy(&wbuf[4], text, len);

    // send request
    if (write_all(fd, wbuf, 4 + len)) return -1;

    char rbuf[4 + MAXCONN];

    // read 4-byte header
    if (read_full(fd, rbuf, 4)) return -1;
    memcpy(&net_len, rbuf, 4);
    len = ntohl(net_len);

    if (len > MAXCONN) return -1;

    // read payload (server reply)
    if (read_full(fd, &rbuf[4], len)) return -1;

    printf("server says: %.*s\n", len, &rbuf[4]);
    return 0;
}
