#include "../include/request.h"

bool process_request(Connection *conn)
{
    if (conn->incoming.size() < 4)
    {
        return false;
    }

    uint32_t len{};
    std::memcpy(&len, conn->incoming.data(), 4);

    // reject long msg size
    if (len > MAX_MSG_SIZE)
    {
        msg("Too Long");
        conn->want_close = true;
        return false; // want close
    }

    if (4 + len > conn->incoming.size())
    {
        return false; // want read
    }

    const uint8_t *request{conn->incoming.data() + 4};

    // parse request, add to buffer

    //  TODO: implement application logic with the request
    printf("client says: len:%d data:%.*s\n", len, len < 100 ? len : 100, request);

    // generate response
    conn->outgoing.append(reinterpret_cast<uint8_t *>(&len), 4);
    conn->outgoing.append(request, len);

    // remove request message
    conn->incoming.consume(4 + len);

    return true;
}

int32_t parse_request()
{
    return 0;
}

void exec_request(std::vector<std::string> &cmd, Buffer &out)
{
    if (cmd.size() == 2 && cmd[0] == "get")
    {
        std::cout << "GET command" << '\n';
    }
    else if (cmd.size() == 3 && cmd[0] == "set")
    {
        std::cout << "SET command" << '\n';
    }
    else if (cmd.size() == 2 && cmd[0] == "del")
    {
        std::cout << "DEL command" << '\n';
    }
    else if (cmd.size() == 1 && cmd[0] == "keys")
    {
        std::cout << "KEYS command" << '\n';
    }
    else
    {
        return output_err(out, static_cast<uint32_t>(Error::ERR_UNKNOWN), "Unkown Command");
    }
}