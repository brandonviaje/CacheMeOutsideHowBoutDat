#include "tcp_server.h"
#include "my_types.h"
#include "utils.h"

static int32_t process_request(int connfd)
{
    char read_buffer[4 + MAXCONN];                          // buffer for header + payload

    // read header
    if(read_full(connfd, read_buffer, 4)) 
    {
        return -1;   
    }

    uint32_t header_len{};                 
    std::memcpy(&header_len, read_buffer, 4);               // copy 4 bytes into header_len
    header_len = ntohl(header_len);                         // convert from network byte order to host

    if(header_len > MAXCONN)                          
    {
        throw std::runtime_error("Header: Too Long"); 
    }

    // read payload
    if(read_full(connfd, &read_buffer[4], header_len))   
    {
        return -1;             
    }

    printf("Client says: %.*s\n", header_len, &read_buffer[4]); 

    // reply
    std::string reply{"world"};          
    uint32_t reply_len{static_cast<uint32_t>(reply.size())};   
    uint32_t net_len{htonl(reply_len)};                     // convert to network byte order

    char write_buffer[4 + MAXCONN];                         // buffer for sending header + payload
    memcpy(write_buffer, &net_len, 4);                      // copy 4-byte length header
    memcpy(&write_buffer[4], reply.data(), reply_len);      // copy actual payload

    return write_all(connfd, write_buffer, 4 + reply_len);  // send everything
}

void create_server_connection()
{
    try
    {
        // create server socket
        int server_socket{socket(AF_INET, SOCK_STREAM, 0)};

        if(server_socket == -1)
        {
            throw std::runtime_error("server socket() failed");
        } 

        // init server address
        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;

        // bind socket
        bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

        std::cout << "Listening for client connections..." << std::endl;

        int status{listen(server_socket, MAXCONN)};

        if(status == -1)
        {
            throw std::runtime_error("listen() failed");
        }
            
        // listen for incoming client connections
        while(true)
        {   

            // accept incoming client connection
            sockaddr_in client_address{};
            socklen_t client_len{sizeof(client_address)};
            int client_connection{accept(server_socket, (struct sockaddr*)&client_address, &client_len)};

            // check for errors
            if(client_connection == -1)
            {
                std::cerr << "accept() failed: " << strerror(errno) << "\n";
                continue; // keep server alive
            }

            std::cout << "Client Connection Accepted!" << std::endl;

            // serve one client connection
            while(true)
            {
                int32_t status{process_request(client_connection)};      // process client request
                if(status)
                {
                    break; // client disconnected or error
                }
            }

            close(client_connection);
            std::cout << "Client disconnected." << std::endl;
        }

        close(server_socket);
    }

    catch(const std::exception& e)
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
