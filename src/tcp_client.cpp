#include "../include/tcp_client.h"

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

        // init server address
        sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
        {
            throw std::runtime_error("inet_pton failed");
        }

        // connect to server
        int status{connect(client_socket, (const sockaddr *)&server_address, sizeof(server_address))};

        // catch error
        if (status == -1)
        {
            throw std::runtime_error("Error connecting to server");
        }

        // multiple pipelined requests
        std::vector<std::string> query_list{
            "hello1", "hello2", "hello3", std::string(MAX_MSG_SIZE, 'z'), "hello5"};

        // send all requests to server
        for (const std::string &s : query_list)
        {
            int32_t err = send_request(client_socket, (uint8_t *)s.data(), s.size());

            if (err)
            {
                close(client_socket);
                return false;
            }
        }

        // read all responses from server
        for (size_t i = 0; i < query_list.size(); ++i)
        {
            int32_t err = read_result(client_socket);

            if (err)
            {
                close(client_socket);
                return false;
            }
        }

        close(client_socket);
        return true;
    }

    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
        return false;
    }
}

int main()
{
    create_client_connection();
    return 0;
}
