#include "tcp_server.h"

void create_server_connection()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    int rv = listen(server_socket, MAXCONN); // listen to incoming connections

    // check for errors
    if (rv < 0)
    {
        std::cout << "Error Creating Socket" << '\n';
    }

    while (true)
    {
        // accept incoming client connection
        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);
        int client_connection = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

        // check for errors
        if (client_connection < 0)
        {
            std::cout << "Error accepting incoming client connection" << '\n';
        }

        close(server_socket); // close server socket
    }
}