#include "tcp_client.h"

void create_client_connection()
{
    // create client socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1)
    {
        throw std::runtime_error("client socket() failed");
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int status = connect(client_socket, (const sockaddr *)&server_address, sizeof(server_address));

    // catch error
    if (status == -1)
    {
        throw std::runtime_error("Error connecting to server");
    }

    close(client_socket);
}