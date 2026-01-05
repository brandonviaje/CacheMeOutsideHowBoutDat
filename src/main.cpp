#include "tcp_server.h"
#include "tcp_client.h"
#include <stdlib.h>

int main()
{
    std::cout << "hello world" << '\n';

    create_client_connection();
    create_server_connection();
    return 0;
}