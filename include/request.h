#ifndef REQUEST_H
#define REQUEST_H

#include "tcp_server.h"
#include "utils.h"
#include "buffer.h"
#include "reponse.h"

struct Connection;

bool process_request(Connection *conn);
int32_t parse_request();
void exec_request(std::vector<std::string> &cmd, Buffer &out);

#endif
