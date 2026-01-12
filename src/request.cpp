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

void get_value(std::vector<std::string> &cmd, Buffer &out)
{
    // create key for lookup
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hash_code = str_hash((uint8_t *)key.key.data(), key.key.size());

    // lookup in hashtable
    HashNode *node{hashmap_lookup(&g_Data.db, &key.node, &entry_eq)};

    if (!node)
        return output_nil(out);

    const std::string &val{container_of(node, Entry, val)->val};
    return output_str(out, val.data());
}

void set_value(std::vector<std::string> &cmd, Buffer &out)
{
    // create key for lookup
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hash_code = str_hash((uint8_t *)key.key.data(), key.key.size());

    // lookup in hashtable
    HashNode *node{hashmap_lookup(&g_Data.db, &key.node, &entry_eq)};

    // if found set new value
    if (node)
    {
        container_of(node, Entry, node)->val.swap(cmd[2]);
    }
    else
    {
        // else insert a new pair
        Entry *entry{new Entry()};
        entry->key.swap(key.key);
        entry->node.hash_code = key.node.hash_code;
        entry->val.swap(cmd[2]);
        hashmap_insert(&g_Data.db, &entry->node);
    }

    return output_nil(out);
}

void del_value(std::vector<std::string> &cmd, Buffer &out)
{
    // create key for lookup
    Entry key;
    key.key.swap(cmd[1]);
    key.node.hash_code = str_hash((uint8_t *)key.key.data(), key.key.size());

    // delete node from hashtable
    HashNode *node{hashmap_delete(&g_Data.db, &key.node, &entry_eq)};

    if (node)
    {
        delete container_of(node, Entry, node);
    }

    return output_int(out, node ? 1 : 0);
}

void exec_request(std::vector<std::string> &cmd, Buffer &out)
{
    if (cmd.size() == 2 && cmd[0] == "get")
    {
        return get_value(cmd, out);
    }
    else if (cmd.size() == 3 && cmd[0] == "set")
    {
        return set_value(cmd, out);
    }
    else if (cmd.size() == 2 && cmd[0] == "del")
    {
        return del_value(cmd, out);
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
