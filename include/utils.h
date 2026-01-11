#ifndef UTILS_H
#define UTILS_H

#include "my_types.h"
#include "buffer.h"
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

void msg(const char *msg);
void set_nonblocking(int fd);
int32_t read_full(int fd, void *buffer, ssize_t total_bytes);
int32_t write_all(int fd, const void *buffer, ssize_t total_bytes);
int32_t send_request(int fd, const uint8_t *text, size_t len);
int32_t read_result(int fd);

#endif
