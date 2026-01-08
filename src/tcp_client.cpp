#include "tcp_client.h"

bool create_client_connection()
{
    try
    {
        // create client socket
        int client_socket{socket(AF_INET, SOCK_STREAM, 0)}; 

        if (client_socket == -1)
        {
            throw std::runtime_error("client socket() failed");
        }

        // initialize server address
        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_port =  htons(PORT);
        
        if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
        {
            throw std::runtime_error("inet_pton failed");
        }

        // connect to server
        int status { connect(client_socket, (const sockaddr *)&server_address, sizeof(server_address)) };

        // catch error
        if (status == -1)
        {
            throw std::runtime_error("Error connecting to server");
        }   
        
        // multiple requests
        if (query(client_socket, "hello1") != 0) return false;
        if (query(client_socket, "hello2") != 0) return false;
        if (query(client_socket, "hello3") != 0) return false;

        close(client_socket);
        return true;
    }

    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return false;
    }
}

int main()
{
    create_client_connection();
    return 0;
}
