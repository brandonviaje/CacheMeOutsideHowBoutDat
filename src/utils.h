#ifndef UTILS_H
#define UTILS_H

#include "my_types.h"
#include <iostream>  
#include <cstdio>      
#include <cstring>     
#include <cassert>     
#include <cerrno>      
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 

void msg(const char* msg);
int32_t read_full(int fd, char* buffer, ssize_t total_bytes);
int32_t write_all(int fd, const char* buffer, ssize_t total_bytes);
int32_t query(int fd, const char *text);

#endif
