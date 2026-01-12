#ifndef REQUEST_H
#define REQUEST_H

#include "tcp_server.h"
#include "utils.h"
#include "buffer.h"
#include "reponse.h"

struct Connection;

bool process_request(Connection *conn);
int32_t parse_request(const uint8_t *data, std::size_t size, std::vector<std::string> &out);
void exec_request(std::vector<std::string> &cmd, Buffer &out);

#endif
