#include "tcp_server.h"
#include "my_types.h"

void create_server_connection()
{

    // create server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        throw std::runtime_error("server socket() failed");
    }

    // init server address
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind socket
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    int status = listen(server_socket, MAXCONN); // listen to incoming connections

    // check for errors
    if (status == -1)
    {
        throw std::runtime_error("Error listening to common connections");
    }

    // event loop to listen to connections
    while (true)
    {
        // accept incoming client connection
        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);
        int client_connection =
            accept(server_socket, (struct sockaddr *)&client_address, &client_len);

        // check for errors
        if (client_connection == -1)
        {
            std::cout << "Error accepting incoming client connection" << '\n';
        }

        close(server_socket); // close server socket
    }
}
