#ifndef RESPONSE_H
#define RESPONSE_H

#include "buffer.h"
#include "utils.h"
#include "my_types.h"
#include <cstdint>

void response_begin(Buffer &out, std::size_t *header);
std::size_t response_size(Buffer &out, std::size_t header);
void response_end(Buffer &out, std::size_t header);

#endif
