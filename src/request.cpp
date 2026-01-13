#include "../include/request.h"

bool process_request(Connection *conn)
{
    // check if we can read msg len
    if (conn->incoming.size() < 4)
    {
        return false;
    }

    uint32_t len{};

    // read first 4 bytes as msg length
    std::memcpy(&len, conn->incoming.data(), 4);
    len = ntohl(len);

    // reject long msgs
    if (len > MAX_MSG_SIZE)
    {
        std::cout << "Message size too long" << '\n';
        conn->want_close = true;
        return false; // want close
    }

    // if we don't have the full msg yet, wait for more data
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
        conn->want_close = true; // invalid request, close conn
        return false;
    }

    std::size_t header_position{};
    response_begin(conn->outgoing, &header_position); // prepare response
    exec_request(cmd, conn->outgoing);                // exec command and write result
    response_end(conn->outgoing, header_position);    // finalize response

    // remove processed request from incoming buffer
    conn->incoming.consume(4 + len);

    return true;
}

int32_t parse_request(const uint8_t *data, std::size_t size, std::vector<std::string> &out)
{
    const uint8_t *end = data + size; // ptr to end of buffer
    uint32_t nstr{};                  // num of strings expected in request

    // read first 4 bytes as the number of strings
    if (!read_u32(data, end, nstr))
        return -1;

    if (nstr > MAX_ARGS)
        return -1;

    // keep reading strings until we've read all
    while (out.size() < nstr)
    {
        uint32_t len{}; // len of next string

        // read next 4 bytes as length of this string
        if (!read_u32(data, end, len))
            return -1;

        out.push_back(std::string()); // add empty string to vector

        // read `len` bytes from the buffer and store it in the last string in `out`
        if (!read_str(data, end, len, out.back()))
            return -1;
    }

    // make sure to consume all the data
    if (data != end)
        return -1;

    return 0;
}

void get_value(std::vector<std::string> &cmd, Buffer &out)
{
    // create key for lookup
    Entry key;
    key.key = cmd[1];
    key.node.hash_code = hash::xxhash64(reinterpret_cast<const uint8_t *>(key.key.data()), key.key.size());

    // lookup in hashtable
    HashNode *node{hashmap_lookup(&g_KeyStore.database, &key.node, &entry_eq)};

    if (!node)
        return output_nil(out);

    const std::string &val{container_of(node, Entry, node)->val};
    return output_str(out, val.data());
}

void set_value(std::vector<std::string> &cmd, Buffer &out)
{
    // create key for lookup
    Entry key;
    key.key = cmd[1];
    key.node.hash_code = hash::xxhash64(reinterpret_cast<const uint8_t *>(key.key.data()), key.key.size());

    // lookup in hashtable
    HashNode *node{hashmap_lookup(&g_KeyStore.database, &key.node, &entry_eq)};

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
        hashmap_insert(&g_KeyStore.database, &entry->node);
    }

    return output_nil(out);
}

void del_value(std::vector<std::string> &cmd, Buffer &out)
{
    // create key for lookup
    Entry key;
    key.key = cmd[1];
    key.node.hash_code = hash::xxhash64(reinterpret_cast<const uint8_t *>(key.key.data()), key.key.size());

    // delete node from hashtable
    HashNode *node{hashmap_delete(&g_KeyStore.database, &key.node, &entry_eq)};

    if (node)
    {
        delete container_of(node, Entry, node);
    }

    return output_int(out, node ? 1 : 0);
}

void list_keys(Buffer &out)
{
    output_arr(out, static_cast<uint32_t>(hashmap_size(&g_KeyStore.database)));   // write num of keys in the database to buffer
    hashmap_foreach(&g_KeyStore.database, &write_key, static_cast<void *>(&out)); // iterate over every key in hasmap, write key into buffer
}

void db_size(Buffer &out)
{
    auto num_keys{static_cast<uint32_t>(hashmap_size(&g_KeyStore.database))}; // count num of keys in database
    output_int(out, num_keys);                                                // write to buffer
}

void exec_request(std::vector<std::string> &cmd, Buffer &out)
{
    if (cmd.empty())
    {
        return output_err(out, static_cast<uint32_t>(Error::ERR_UNKNOWN), "Empty Command");
    }

    // convert to lowercase
    std::string command{cmd[0]};
    std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c)
                   { return std::tolower(c); });

    if (cmd.size() == 2 && command == "get")
    {
        return get_value(cmd, out);
    }
    else if (cmd.size() == 3 && command == "set")
    {
        return set_value(cmd, out);
    }
    else if (cmd.size() == 2 && command == "del")
    {
        return del_value(cmd, out);
    }
    else if (cmd.size() == 1 && command == "keys")
    {
        return list_keys(out);
    }
    else if (cmd.size() == 1 && command == "dbsize")
    {
        return db_size(out);
    }
    else
    {
        return output_err(out, static_cast<uint32_t>(Error::ERR_UNKNOWN), "Unknown Command");
    }
}
