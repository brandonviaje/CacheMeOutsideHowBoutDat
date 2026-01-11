#ifndef TCP_SERVER
#define TCP_SERVER

#include "my_types.h"
#include "utils.h"
#include "buffer.h"
#include <string>
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

struct Connection
{
    int fd = -1;
    bool want_read = false;
    bool want_write = false;
    bool want_close = false;
    Buffer incoming;
    Buffer outgoing;
};

bool process_request(Connection *conn);
Connection *handle_accept(int fd);
void handle_read(Connection *conn);
void handle_write(Connection *conn);
void create_server_connection();

#endif
