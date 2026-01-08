#ifndef TCP_CLIENT
#define TCP_CLIENT

#include "my_types.h"
#include "utils.h"
#include <unistd.h>     

#include <iostream>    
#include <stdexcept>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <errno.h>     

bool create_client_connection();

#endif
