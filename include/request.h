#ifndef REQUEST_H
#define REQUEST_H

#include "hash_map.h"
#include "my_types.h"
#include "tcp_server.h"
#include "utils.h"
#include "buffer.h"
#include "reponse.h"

struct Connection;

bool process_request(Connection *conn);
int32_t parse_request(const uint8_t *data, std::size_t size, std::vector<std::string> &out);
void get_value(std::vector<std::string> &cmd, Buffer &out);
void set_value(std::vector<std::string> &cmd, Buffer &out);
void del_value(std::vector<std::string> &cmd, Buffer &out);
void exec_request(std::vector<std::string> &cmd, Buffer &out);

#endif
