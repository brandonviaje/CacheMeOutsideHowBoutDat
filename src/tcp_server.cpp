#include "../include/tcp_server.h"
#include "../include/my_types.h"
#include "../include/utils.h"

Connection *handle_accept(int fd)
{
    sockaddr_in client_address{};
    socklen_t address_len{sizeof(client_address)};
    int conn_fd{accept(fd, (sockaddr *)&client_address, &address_len)};

    if (conn_fd == -1)
    {
        std::cerr << "Error accepting connection" << '\n';
        return NULL;
    }

    // set connection to non blocking FD
    set_nonblocking(conn_fd);

    // create new struct, init and return
    Connection *conn{new Connection()};
    conn->fd = conn_fd;
    conn->want_read = true;

    return conn;
}

void handle_read(Connection *conn)
{
    uint8_t buffer[64 * 1024];
    ssize_t status{recv(conn->fd, buffer, sizeof(buffer), 0)};

    if (status == -1)
    {
        std::cerr << "Error recieving message" << '\n';
        conn->want_close = true;
        return;
    }

    if (status == 0)
    {
        if (conn->incoming.size() == 0)
        {
            std::cout << "Client closed." << '\n';
        }
        else
        {
            std::cout << "EOF" << '\n';
        }
        conn->want_close = true;
        return;
    }

    conn->incoming.append(buffer, static_cast<size_t>(status));

    while (process_request(conn))
    {
    }

    if (conn->outgoing.size() > 0)
    {
        conn->want_read = false;
        conn->want_write = true;
        return handle_write(conn);
    }
}

void handle_write(Connection *conn)
{
    assert(conn->outgoing.size() > 0);

    // check if outgoing is empty
    size_t out_size{conn->outgoing.size()};

    ssize_t status{send(conn->fd, conn->outgoing.data(), out_size, 0)};

    // check if socket is ready
    if (status == -1 && status == EAGAIN)
    {
        return;
    }

    // handle error
    if (status == -1)
    {
        std::cerr << "Error sending bytes to stream" << '\n';
        conn->want_close = true;
        return;
    }

    // remove written data from outgoing
    conn->outgoing.consume(static_cast<size_t>(status));

    // update readiness
    if (out_size == 0)
    {
        conn->want_write = false;
        conn->want_read = true;
        return;
    }
}

void create_server_connection()
{
    try
    {
        // create server socket
        int server_socket{socket(AF_INET, SOCK_STREAM, 0)};

        if (server_socket == -1)
        {
            throw std::runtime_error("server socket() failed");
        }

        // init server address
        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;

        // bind socket
        bind(server_socket, (const sockaddr *)&server_address, sizeof(server_address));

        set_nonblocking(server_socket); // set fd to nonblocking

        std::cout << "Listening for client connections..." << std::endl;

        if (listen(server_socket, MAXCONN) == -1)
        {
            throw std::runtime_error("listen() failed");
        }

        // client connections
        std::vector<Connection *> client_connections;

        // event loop
        std::vector<pollfd> poll_args;

        // listen for incoming client connections
        while (true)
        {

            poll_args.clear();                              // clear poll vector
            struct pollfd pfd = {server_socket, POLLIN, 0}; // add server socket first so we can accept new connections
            poll_args.push_back(pfd);

            // loop through client connections
            for (Connection *conn : client_connections)
            {
                if (!conn) // skip if the pointer is null
                {
                    continue;
                }

                struct pollfd pfd = {conn->fd, POLLERR, 0};

                // if this conn wants to read add POLLIN flag
                if (conn->want_read)
                {
                    pfd.events |= POLLIN;
                }

                // if this conn wants to write add POLLOUT flag
                if (conn->want_write)
                {
                    pfd.events |= POLLOUT;
                }

                // add client socket to poll list
                poll_args.push_back(pfd);
            }

            // wait for readiness
            int status{poll(poll_args.data(), static_cast<nfds_t>(poll_args.size()), -1)};

            if (status == -1 && errno == EINTR)
            {
                continue;
            }

            if (status == -1)
            {
                throw std::runtime_error("Error polling for events");
            }

            // handle listening socket
            if (poll_args[0].revents)
            {

                Connection *conn = handle_accept(server_socket);

                if (conn)
                {
                    // put it into map
                    if (client_connections.size() <= (size_t)conn->fd)
                    {
                        client_connections.resize(conn->fd + 1);
                    }
                    assert(!client_connections[conn->fd]);
                    client_connections[conn->fd] = conn;
                }
            }

            // handle connection sockets
            for (size_t i{1}; i < poll_args.size(); ++i)
            {
                uint32_t ready{static_cast<uint32_t>(poll_args[i].revents)};

                if (ready == 0)
                {
                    continue;
                }

                Connection *conn{client_connections[poll_args[i].fd]};

                // handle read if available
                if (ready & POLLIN)
                {
                    assert(conn->want_read);
                    handle_read(conn);
                }

                // handle write if available
                if (ready & POLLOUT)
                {
                    assert(conn->want_write);
                    handle_write(conn);
                }

                // close socket from err or app logic
                if (ready & POLLERR || conn->want_close)
                {
                    static_cast<void>(close(conn->fd));
                    client_connections[conn->fd] = NULL;
                    delete conn;
                }
            }
        }

        close(server_socket);
    }

    catch (const std::exception &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

int main()
{
    std::cout << "Starting Server..." << std::endl;
    create_server_connection();
    return 0;
}
