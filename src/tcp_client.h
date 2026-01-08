#ifndef TCP_CLIENT
#define TCP_CLIENT

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

bool create_client_connection();

#endif
