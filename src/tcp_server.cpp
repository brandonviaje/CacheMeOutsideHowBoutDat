#include "tcp_server.h"
#include "my_types.h"


bool create_server_connection() 
{
    try
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
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;

        // bind socket
        bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
        
        std::cout << "Listening for client connections..." << '\n';

        // listen to incoming connections
        while (true) 
        {
            int status = listen(server_socket, MAXCONN);
        
            // check for errors
            if (status == -1) 
            {
                throw std::runtime_error("Error listening to client connections");
            }

            // accept incoming client connection
            sockaddr_in client_address{};
            socklen_t client_len = sizeof(client_address);
            int client_connection = accept(server_socket, (struct sockaddr *) &client_address, &client_len);

            // check for errors
            if (client_connection == -1) 
            {
                throw std::runtime_error("failed to accept client connection");
            }

            std::cout << "Client Connection Accepted!" << '\n';
        }

        close(server_socket); // close server socket
        return true;
    }

    catch (const std::runtime_error& e) 
    {
        std::cerr << "Runtime error: " << e.what() << '\n';
        return false;   
    }

    catch (const std::exception& e) 
    {
        std::cerr << "Exception: " << e.what() << '\n';
        return false;
    }
}

int main() 
{

    std::cout << "Starting Server..." << '\n';

    if(create_server_connection())
        std::cout << "Server Creation Succesful" << '\n';
    else
        std::cout << "Server Connectikon Failed" << '\n';
    
    return 0;
}
