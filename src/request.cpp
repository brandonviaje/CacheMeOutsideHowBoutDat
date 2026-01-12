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

    std::vector<std::string> cmd;

    // parse request, add to buffer
    if (parse_request(request, len, cmd) == -1)
    {
        std::cout << "Bad Request" << '\n';
        conn->want_close = true;
        return false;
    }

    std::size_t header_position{};
    response_begin(conn->outgoing, &header_position);
    exec_request(cmd, conn->outgoing);
    response_end(&conn->outgoing, header_position);

    // remove request message
    conn->incoming.consume(4 + len);

    return true;
}

int32_t parse_request(const uint8_t *data, std::size_t size, std::vector<std::string> &out)
{
    const uint8_t *end = data + size;
    uint32_t nstr{};

    if (!read_u32(data, end, nstr))
        return -1;

    if (nstr > MAX_ARGS)
        return -1;

    while (out.size() < nstr)
    {
        uint32_t len{};
        if (!read_u32(data, end, len))
            return -1;
        out.push_back(std::string());

        if (!read_str(data, end, len, out.back()))
            return -1;
    }

    if (data != end)
        return -1;

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
