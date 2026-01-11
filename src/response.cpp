#include "../include/reponse.h"

void response_begin(Buffer &out, std::size_t *header)
{
    *header = out.size();
    buffer_append_u32(out, *header);
}

std::size_t response_size(Buffer &out, std::size_t header)
{
    return out.size() - header - 4;
}

void response_end(Buffer *out, std::size_t header)
{
    std::size_t msg_size{response_size(*out, header)};

    if (msg_size > MAX_MSG_SIZE)
    {
        out->resize(header + 4);
        output_err(*out, static_cast<uint32_t>(Error::ERR_TOO_BIG), "Message is Too Long.");
        msg_size = response_size(*out, header);
    }

    uint32_t len{static_cast<uint32_t>(msg_size)};
    out[header] = len;
}
