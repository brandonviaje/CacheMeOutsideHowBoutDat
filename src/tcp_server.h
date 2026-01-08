#ifndef TCP_SERVER
#define TCP_SERVER

#include "my_types.h"   
#include "utils.h"      
#include <iostream>     
#include <string>       
#include <stdexcept>    
#include <cstring>      
#include <cstdio>      
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>     
#include <errno.h>     

void create_server_connection();

#endif
