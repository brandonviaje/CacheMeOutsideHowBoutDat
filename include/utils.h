#ifndef UTILS_H
#define UTILS_H

#include "buffer.h"
#include "my_types.h"
#include <cstring>
#include <string>
#include <cstdint>
#include <cassert>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

void msg(const char *msg);
void set_nonblocking(int fd);
int32_t print_response(const uint8_t *data, size_t size);
int32_t read_full(int fd, void *buffer, ssize_t total_bytes);
int32_t write_all(int fd, const void *buffer, ssize_t total_bytes);
int32_t send_request(int fd, const std::vector<std::string> &cmd);
int32_t read_result(int fd);

void buffer_append_u8(Buffer &buffer, uint8_t data);
void buffer_append_u32(Buffer &buffer, uint32_t data);
void buffer_append_i64(Buffer &buffer, int64_t data);
void buffer_append_f64(Buffer &buffer, double data);
void buffer_append(Buffer &buffer, const uint8_t *data, std::size_t n);

void output_str(Buffer &out, const std::string &s);
void output_int(Buffer &out, int64_t val);
void output_arr(Buffer &out, uint32_t n);
void output_dbl(Buffer &out, double val);
void output_nil(Buffer &out);
void output_err(Buffer &out, uint32_t code, const std::string &msg);

bool read_u32(const uint8_t *&cur, const uint8_t *end, uint32_t &out);
bool read_str(const uint8_t *&cur, const uint8_t *end, size_t n, std::string &out);

#endif
