#include "../include/tcp_client.h"

bool create_client_connection(int argc, char **argv)
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

        std::vector<std::string> cmd;

        for (int i = 1; i < argc; ++i)
        {
            cmd.push_back(argv[i]);
        }

        // send request
        int32_t err = send_request(client_socket, cmd);

        if (err == -1)
        {
            close(client_socket);
            return false;
        }

        err = read_result(client_socket);

        if (err == -1)
        {
            close(client_socket);
            return false;
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

int main(int argc, char **argv)
{
    create_client_connection(argc, argv);
    return 0;
}
