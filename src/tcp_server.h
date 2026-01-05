#ifndef TCP_SERVER
#define TCP_SERVER

#include <stdlib.h>
#include <stdint.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

void create_server_connection();

#endif