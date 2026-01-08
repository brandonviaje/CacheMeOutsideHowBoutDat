#include "tcp_client.h"
#include "my_types.h"

bool create_client_connection()
{
    try
    {
        // create client socket
        int client_socket = socket(AF_INET, SOCK_STREAM, 0); // af_inet is IPV4, SOCK_STREAM is TCP, protocol is automatically chosen

        if (client_socket == -1)
        {
            throw std::runtime_error("client socket() failed");
        }

        // initialize server address
        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        
        if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
        {
            throw std::runtime_error("inet_pton failed");
        }

        // connect to server
        int status = connect(client_socket, (const sockaddr *)&server_address, sizeof(server_address));

        // catch error
        if (status == -1)
        {
            throw std::runtime_error("Error connecting to server");
        }   

        // create a byte buffer
        close(client_socket);
        return true;
    }

    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << '\n';
        return false;   
    }
    
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return false;
    }
}

int main()
{
    std::cout << "Starting Client...\n";
    if (create_client_connection())
        std::cout << "Client Connection Created\n";
    else
        std::cout << "Client Connection Failed\n";
    return 0;
}
