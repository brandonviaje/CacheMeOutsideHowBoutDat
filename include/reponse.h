#ifndef RESPONSE_H
#define RESPONSE_H

#include "buffer.h"
#include "my_types.h"
#include <cstdint>

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
void response_begin(Buffer &out, std::size_t *header);
std::size_t response_size(Buffer &out, std::size_t header);
void response_end(Buffer *out, std::size_t header);

#endif