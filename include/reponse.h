#ifndef RESPONSE_H
#define RESPONSE_H

#include "buffer.h"
#include "my_types.h"
#include <cstdint>

void buffer_append_u8(Buffer &buffer, uint8_t data);
void buffer_append_u32(Buffer &buffer, uint32_t data);
void buffer_append_i64(Buffer &buffer, int64_t data);
void buffer_append_double(Buffer &buffer, double data);
void buffer_append(Buffer &buffer, uint8_t data);

#endif